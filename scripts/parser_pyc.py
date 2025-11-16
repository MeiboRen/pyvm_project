import sys
import types
import marshal
import dis
import binascii

def print_hex(label, string, tab):
    number = binascii.hexlify(string).decode('ascii')
    if len(number) >= 100:
        print(f"{tab}{label}:")
        for i in range(0, len(number), 100):
            print(f"{tab}   {number[i:i+100]}")
    else:
        print(f"{tab}{label} {number}")

def parse_code(code, tab=''):
    print(f"{tab}code:")
    tab += '   '
    print(f"{tab}argcount {code.co_argcount}")
    print(f"{tab}posonlyargcount {code.co_posonlyargcount}")
    print(f"{tab}kwonlyargcount {code.co_kwonlyargcount}")
    print(f"{tab}nlocals {code.co_nlocals}")
    print(f"{tab}stacksize {code.co_stacksize}")
    print(f"{tab}flags {code.co_flags}")

    # print_hex("code", code.co_code, tab=tab)
    print(f"{tab}code:")
    dis.dis(code)

    print(f"{tab}consts:")
    for const in code.co_consts:
        if type(const) == types.CodeType:
            parse_code(const, tab+'   ')
        else:
            print(f"   {tab}{const}")

    print(f"{tab}names {code.co_names}")
    print(f"{tab}varnames {code.co_varnames}")
    print(f"{tab}freevars {code.co_freevars}")
    print(f"{tab}cellvars {code.co_cellvars}")
    print(f"{tab}filename {code.co_filename}")
    print(f"{tab}name {code.co_name}")
    print(f"{tab}firstlineno {code.co_firstlineno}")
    if hasattr(code, 'co_linetable'):
        print_hex("linetable", code.co_linetable, tab=tab)

def parse_file(filename):
    with open(filename, "rb") as f:
        magic_number = f.read(4)
        zero_number = f.read(4)
        timestamp_number = f.read(4)
        size_number = f.read(4)
        print(f"magic number {binascii.hexlify(magic_number).decode('ascii')}")
        print(f"timestamp {binascii.hexlify(timestamp_number).decode('ascii')}")
        print(f"size {binascii.hexlify(size_number).decode('ascii')}")

        code = marshal.load(f)
        parse_code(code)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        parse_file(sys.argv[1])
    else:
        print("usage: python3 scripts/parser_pyc.py <pyc_file>")