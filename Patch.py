with open('Cube.exe', 'r+b') as f:
    with open('Cube.exe.bak', 'wb') as backup:
        backup.write(f.read())
    f.seek(0x4417E)
    f.write(b'\xAC')
    
