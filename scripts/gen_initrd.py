import sys
import struct
import os

# Format:
# Header: 
#   uint32_t magic = 0xBF5C
#   uint32_t num_files
# For each file:
#   char name[32]
#   uint32_t offset (from start of file)
#   uint32_t size
# Data follows...

def main():
    if len(sys.argv) < 3:
        print("Usage: gen_initrd.py <output> <input1> <input2> ...")
        return
        
    out_file = sys.argv[1]
    inputs = sys.argv[2:]
    
    headers = []
    data_blocks = []
    
    data_offset = 8 + len(inputs) * 40
    
    for infile in inputs:
        name = os.path.basename(infile)
        if len(name) >= 32:
            name = name[:31]
            
        with open(infile, 'rb') as f:
            data = f.read()
            
        size = len(data)
        
        name_bytes = name.encode('ascii').ljust(32, b'\x00')
        headers.append((name_bytes, data_offset, size))
        data_blocks.append(data)
        
        data_offset += size
        
    with open(out_file, 'wb') as out:
        out.write(struct.pack('<II', 0xBF5C, len(inputs)))
        for name_bytes, offset, size in headers:
            out.write(name_bytes)
            out.write(struct.pack('<II', offset, size))
        for data in data_blocks:
            out.write(data)

if __name__ == '__main__':
    main()
