#!/usr/bin/env python3
"""verifyDat.py

Reads a Manic Miner map file and splits it into named sections.
Each section starts with a line containing exactly "name{" and ends with a line containing exactly "}".

Usage:
    python verifyDat.py <filename>

Output:
    Prints a summary of found sections and their line ranges.

This is the first step: reading and parsing sections. Validation per-section will be added later.
"""

from __future__ import annotations

import sys
from typing import Dict, List, Tuple
import io
import locale


SECTION_NAMES = [
    "comments",
    "info",
    "tiles",
    "height",
    "resources",
    "objectives",
    "buildings",
    "landslidefrequency",
    "lavaspread",
    "miners",
    "briefing",
    "briefingsuccess",
    "briefingfailure",
    "vehicles",
    "creatures",
    "blocks",
    "script",
]


def detect_bom_encoding(data: bytes) -> str:
    # Define known BOMs and their corresponding encoding strings
    boms = {
        b'\xef\xbb\xbf': 'utf-8',       # UTF-8 BOM
        b'\xff\xfe': 'utf-16',          # UTF-16 LE
        b'\xfe\xff': 'utf-16',          # UTF-16 BE
        b'\xff\xfe\x00\x00': 'utf-32',  # UTF-32 LE
        b'\x00\x00\xfe\xff': 'utf-32',  # UTF-32 BE
    }

    # Check for each BOM in order of longest to shortest
    for bom, encoding in sorted(boms.items(), key=lambda x: len(x[0]), reverse=True):
        if data.startswith(bom):
            return encoding

    # No BOM found
    return ''

def detect_nonbom_encoding(data: bytes) -> str:
    if len(data) >= 4:
        # UTF-32 LE: [char, 0x00, 0x00, 0x00]
        if 1 <= data[0] <= 127 and data[1:4] == b'\x00\x00\x00':
            return 'utf-32-le'
        # UTF-32 BE: [0x00, 0x00, 0x00, char]
        elif data[0:3] == b'\x00\x00\x00' and 1 <= data[3] <= 127:
            return 'utf-32-be'

    if len(data) >= 2:
        # UTF-16 LE: [char, 0x00]
        if 1 <= data[0] <= 127 and data[1] == 0x00:
            return 'utf-16-le'
        # UTF-16 BE: [0x00, char]
        elif data[0] == 0x00 and 1 <= data[1] <= 127:
            return 'utf-16-be'

    if len(data) >= 1:
        # UTF-8: [char] (single byte)
        if 1 <= data[0] <= 127:
            return 'utf-8'

    return ''

# Check if the file contains only valid UTF-8 sequences.
def is_valid_utf8(bytes_data) -> bool:
    try:
        bytes_data.decode('utf-8')
        return True
    except UnicodeDecodeError:
        return False


def read_file_any_encoding(path: str) -> str:
    """Read a file that may be in any common encoding.
    Strategy:
    - Read raw bytes.
    - look for BOM, if found, decode using BOM.
    - if no BOM, try to infer encoding from starting bytes and decode accordingly.
    - ansi 7 bit is treated as ansii, and if valid utf-8, use utf-8.
    - if utf-8 fails then use system preferred encoding.
    """
    with open(path, "rb") as f:
        data = f.read()

    # First try BOM detection
    encodings = detect_bom_encoding(data)
    if encodings:
        try:
            result = data.decode(encodings)
            print(f"Format: {encodings} BOM decoded: {path}")
            return result
        except Exception as e:
            print(f"Error: Failed UTF decoding of {encodings}: {e}")
            return ''

    # No BOM found, try to detect encoding from content
    encodings = detect_nonbom_encoding(data)        
    if encodings == 'utf-8':   # we have to deal with UTF-8 specifically since it might be windows current code page
        if all(b < 0x80 for b in data):  # all data is 7 bit ASCII
            encodings = 'ansii'
        elif is_valid_utf8(data):
            encodings = 'utf-8'
        else:
            encodings = locale.getpreferredencoding(False)   # windows current code page
    try:
        result = data.decode(encodings)
        print(f"Format: {encodings} NO BOM decoded: {path}")
        return result
    except Exception as e:
        print(f"Error: Failed decoding of {encodings}: {e}")
    return ''


def parse_sections(text: str) -> Dict[str, List[str]]:
    """Parse the file text into sections.

    Returns a dict mapping section name -> list of lines inside that section.
    Errors and exits if:
    - A duplicate section is found
    - An unknown section name is encountered
    Lines keep their original content except we strip the trailing newlines.
    """
    lines = text.splitlines()
    sections: Dict[str, List[str]] = {}
    known_sections = {name.lower() for name in SECTION_NAMES}

    current_name = None
    current_lines: List[str] = []
    start_line_no = None

    def flush_section():
        nonlocal current_name, current_lines, start_line_no
        if current_name is None:
            return
        key = current_name.lower()
        # Check for unknown sections
        if key not in known_sections:
            print(f"ERROR: Unknown section '{current_name}' found at line {start_line_no}")
            print(f"Valid section names are: {', '.join(sorted(SECTION_NAMES))}")
            sys.exit(1)
        # Check for duplicates
        if key in sections:
            print(f"ERROR: duplicate section '{current_name}' found at line {start_line_no}")
            sys.exit(1)
        sections[key] = current_lines
        current_name = None
        current_lines = []
        start_line_no = None

    for idx, raw in enumerate(lines, start=1):
        line = raw.strip()
        # detect start: must be exactly name{ with no extra whitespace? The prompt said "name{ by itself on a single line" so allow whitespace around
        if current_name is None:
            # try to match pattern: <name>{
            if line.endswith("{"):
                name = line[:-1].strip()
                if name:
                    current_name = name
                    current_lines = []
                    start_line_no = idx
                    continue
        else:
            # inside a section; end if line is exactly '}'
            if line == "}":
                flush_section()
                continue
            # otherwise append original raw line (preserve internal spacing)
            current_lines.append(raw)

    # If file ends while inside a section, flush anyway
    if current_name is not None:
        flush_section()

    return sections


def parse_info_section(lines: List[str]) -> Dict[str, str]:
    """Parse the info section which contains key:value pairs.
    
    Format:
    - One key:value pair per line
    - No spaces around the colon
    - Both key and value are trimmed of whitespace
    
    Required keys and validation:
    - rowcount: integer 3-10000
    - colcount: integer 3-10000
    
    Returns:
    - Dictionary mapping keys to values
    - Keys are case-sensitive as specified in the file
    """
    info = {}
    for line_num, line in enumerate(lines, start=1):
        line = line.strip()
        if not line:  # skip empty lines
            continue
            
        if ':' not in line:
            print(f"ERROR: Invalid info section line {line_num}: Missing colon in '{line}'")
            sys.exit(1)
            
        # Split on first colon in case value contains colons
        parts = line.split(':', 1)
        if len(parts) != 2:
            print(f"ERROR: Invalid info section line {line_num}: Malformed key:value pair '{line}'")
            sys.exit(1)
            
        key, value = parts[0].strip(), parts[1].strip()
        if not key:
            print(f"ERROR: Invalid info section line {line_num}: Empty key in '{line}'")
            sys.exit(1)
            
        if ' ' in key:
            print(f"ERROR: Invalid info section line {line_num}: Key contains spaces '{key}'")
            sys.exit(1)
        
        if key in info:
            print(f"ERROR: Duplicate key '{key}' in info section at line {line_num}")
            sys.exit(1)
            
        info[key] = value
    
    # Validate required dimension keys
    for required_key in ['rowcount', 'colcount']:
        if required_key not in info:
            print(f"ERROR: Missing required key '{required_key}' in info section")
            sys.exit(1)
        
        try:
            value = int(info[required_key])
            if not (3 <= value <= 10000):
                print(f"ERROR: {required_key} value {value} must be between 3 and 10000")
                sys.exit(1)
        except ValueError:
            print(f"ERROR: {required_key} value '{info[required_key]}' must be an integer")
            sys.exit(1)
    
    return info


def summarize_sections(sections: Dict[str, List[str]]) -> None:
    print("Found sections:")
    for name, lines in sections.items():
        print(f"  {name}: {len(lines)} lines")
        
        # Special handling for info section
        if name.lower() == 'info':
            try:
                info_data = parse_info_section(lines)
                print("\nInfo section contents:")
                for key, value in sorted(info_data.items()):
                    print(f"  {key}: {value}")
            except Exception as e:
                print(f"Error parsing info section: {e}")


def main(argv: List[str]) -> int:
    if len(argv) != 2:
        print("ERROR: Usage: python verifyDat.py <filename>")
        return 2
    path = argv[1]
    try:
        text = read_file_any_encoding(path)
    except FileNotFoundError:
        print(f"ERROR: File not found: {path}")
        return 3
    if not text:
        print(f"ERROR: Error reading file or file is empty: {path}")
        return 4

    sections = parse_sections(text)
    summarize_sections(sections)
    # print a small sample per known section
    for s in SECTION_NAMES:
        key = s.lower()
        if key in sections:
            print(f"\n--- {s} (first 10 lines) ---")
            for ln in sections[key][:10]:
                print(ln)

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
