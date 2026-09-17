import math
import random

W = 400
H = 300

def color(r, g, b):
    return (r << 16) | (g << 8) | b

def gen_wp1():
    # Abstract tech: smooth gradient with diagonal lines
    data = []
    for y in range(H):
        for x in range(W):
            r = int((x / W) * 50 + 10)
            g = int((y / H) * 100 + 20)
            b = int(((x+y)/(W+H)) * 150 + 50)
            
            # Diagonal line grid
            if (x + y) % 40 < 2:
                r = min(255, r + 50)
                g = min(255, g + 50)
                b = min(255, b + 50)
                
            data.append(color(r, g, b))
    return data

def gen_wp2():
    # Minimalist landscape: Sky gradient with sun and hills
    data = []
    sun_x, sun_y, sun_r = 100, 100, 40
    for y in range(H):
        for x in range(W):
            r = int(10 + (y / H) * 50)
            g = int(20 + (y / H) * 30)
            b = int(40 + (y / H) * 100)
            
            # Sun
            dist = math.sqrt((x - sun_x)**2 + (y - sun_y)**2)
            if dist < sun_r:
                r, g, b = 255, 200, 100
                
            # Hills
            if y > H - 50 + math.sin(x / 40.0) * 20:
                r, g, b = 10, 30, 20
                
            data.append(color(r, g, b))
    return data

def gen_wp3():
    # Cyberpunk grid
    data = []
    for y in range(H):
        for x in range(W):
            # Base dark purple
            r, g, b = 20, 0, 40
            
            # Grid
            if y > H / 2:
                persp_x = (x - W/2) * (y / (H/2)) + W/2
                if int(persp_x) % 30 < 2 or y % 20 < 2:
                    r, g, b = 255, 0, 255 # Neon pink
            else:
                # Sky gradient
                r, g, b = int(40 - (y/(H/2))*40), 0, int(80 - (y/(H/2))*80)
                
            data.append(color(r, g, b))
    return data

def gen_wp4():
    # Space galaxy: Noise + stars
    random.seed(42)
    data = []
    for y in range(H):
        for x in range(W):
            # Nebula
            r = int(math.sin(x/50.0) * math.cos(y/40.0) * 30 + 30)
            g = int(math.sin(x/70.0 + 1) * math.cos(y/60.0 + 1) * 20 + 20)
            b = int(math.sin(x/30.0 + 2) * math.cos(y/50.0 + 2) * 50 + 50)
            
            # Stars
            if random.random() < 0.005:
                r, g, b = 255, 255, 255
                
            data.append(color(r, g, b))
    return data

wps = [gen_wp1(), gen_wp2(), gen_wp3(), gen_wp4()]

with open("gui/wallpapers.c", "w") as f:
    f.write("#include <stdint.h>\n\n")
    for i, wp in enumerate(wps):
        f.write(f"const uint32_t wallpaper_{i+1}[400 * 300] = {{\n")
        # Write chunks of 10 for better formatting
        for j in range(0, len(wp), 10):
            chunk = wp[j:j+10]
            f.write("    " + ", ".join([f"0x{c:06X}" for c in chunk]) + ",\n")
        f.write("};\n\n")

with open("include/wallpapers.h", "w") as f:
    f.write("#ifndef WALLPAPERS_H\n#define WALLPAPERS_H\n\n#include <stdint.h>\n\n")
    for i in range(4):
        f.write(f"extern const uint32_t wallpaper_{i+1}[400 * 300];\n")
    f.write("\n#endif\n")

print("Generated gui/wallpapers.c and include/wallpapers.h successfully.")
