import sys

def detect_utf_encoding(file_path):
    with open(file_path, 'rb') as f:
        raw = f.read(4)

    # BOM signatures
    boms = {
        b'\xef\xbb\xbf': 'UTF-8 with BOM',
        b'\xff\xfe': 'UTF-16 LE with BOM',
        b'\xfe\xff': 'UTF-16 BE with BOM',
        b'\xff\xfe\x00\x00': 'UTF-32 LE with BOM',
        b'\x00\x00\xfe\xff': 'UTF-32 BE with BOM'
    }

    for bom, encoding in boms.items():
        if raw.startswith(bom):
            return encoding

    # No BOM detected, try to infer encoding
    with open(file_path, 'rb') as f:
        raw = f.read(8)

    for encoding in ['utf-8', 'utf-16-le', 'utf-16-be', 'utf-32-le', 'utf-32-be']:
        try:
            text = raw.decode(encoding)
            if text[:2].isalpha():
                if encoding == 'utf-8':
                    return 'UTF8 or ANSI'
                return f'{encoding.upper()} (no BOM)'
        except UnicodeDecodeError:
            continue

    return 'Unknown encoding'

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python detect_encoding.py <filename>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        result = detect_utf_encoding(filename)
        print(result)
    except Exception as e:
        print(f"Error: {e}")