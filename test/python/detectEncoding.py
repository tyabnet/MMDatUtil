import sys

# not using built in decoders, implement detection manually
def detect_utf_encoding(file_path):

    def is_valid_ansi7(bytes_data):
        return all(b < 0x80 for b in bytes_data)

    # Check if the file contains only valid UTF-8 sequences.
    def is_valid_utf8(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-8')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-16 sequences.
    def is_valid_utf16(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-16')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-32 sequences.
    def is_valid_utf32(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-32')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-16LE sequences. BOM must not be present.
    def is_valid_utf16LE(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-16-le')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-32LE sequences. BOM must not be present.
    def is_valid_utf32LE(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-32-le')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-16BE sequences. BOM must not be present.
    def is_valid_utf16BE(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-16-be')
            return True
        except UnicodeDecodeError:
            return False

    # Check if the file contains only valid UTF-32BE sequences. 
    def is_valid_utf32BE(bytes_data) -> bool:
        try:
            bytes_data.decode('utf-32-be')
            return True
        except UnicodeDecodeError:
            return False

    # Read the start of the file to check for BOMs
    try:
        with open(file_path, 'rb') as f:
            start = f.read()   # read entire file for further checks
            if len(start) < 4:
                return 'ERROR file too short'
    except FileNotFoundError:
        return 'ERROR Missing file'
    except Exception:
        return 'ERROR Empty file'

    # BOM signatures
    boms = [
        ( b'\xef\xbb\xbf', 'UTF-8 with BOM', is_valid_utf8 ),
        ( b'\xff\xfe', 'UTF-16 LE with BOM', is_valid_utf16 ),
        ( b'\xfe\xff', 'UTF-16 BE with BOM', is_valid_utf16 ),
        ( b'\xff\xfe\x00\x00', 'UTF-32 LE with BOM', is_valid_utf32 ),
        ( b'\x00\x00\xfe\xff', 'UTF-32 BE with BOM', is_valid_utf32 )
    ]

    for bom, encoding, isValid in boms:
        if start.startswith(bom):
            return encoding if isValid(start) else 'ERROR ' + encoding + ' (invalid sequences detected)'

    # infer encoding if no BOM is found. The file is assumed to be text and it starts with an ANSI character.    
    if 0 < start[0] < 0x80:  # low byte is ASCII range could be any of the LE formats
        if start[1:4] == b'\x00\x00\x00':  # if next 3 bytes are 0, it's likely UTF-32 LE
            return 'UTF-32 LE NO BOM' if is_valid_utf32LE(start[4:]) else 'ERROR UTF-32 LE NO BOM (invalid sequences detected)'
        elif start[1] == 0x00:      # if next byte is 0, it's likely UTF-16 LE
            return 'UTF-16 LE NO BOM' if is_valid_utf16LE(start[2:]) else 'ERROR UTF-16 LE NO BOM (invalid sequences detected)'
    elif start[0] == 0x00:        # if first byte is 0, could be BE formats
        if start[1:3] == b'\x00\x00' and 0 < start[3] < 0x80:  # if next 2 bytes are 0 and last byte is ASCII range, it's likely UTF-32 BE
            return 'UTF-32 BE NO BOM' if is_valid_utf32BE(start[4:]) else 'ERROR UTF-32 BE NO BOM (invalid sequences detected)'
        elif start[1] > 0 and start[1] < 0x80:  # if second byte is ASCII range, it's likely UTF-16 BE
            return 'UTF-16 BE NO BOM' if is_valid_utf16BE(start[2:]) else 'ERROR UTF-16 BE NO BOM (invalid sequences detected)'
   
    # Check if all bytes are < 0x80 (pure ASCII/ANSI)
    if is_valid_ansi7(start):
        return 'ANSI7'


    return 'UTF-8 NO BOM' if is_valid_utf8(start) else 'ANSI8 Windows Code Page'


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("ERROR: Usage: python detectEncoding.py <filename>")
        sys.exit(1)
    filename = sys.argv[1]
    result = detect_utf_encoding(filename)
    print(result)

