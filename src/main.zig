const doslike = @cImport(@cInclude("dos.h"));
const std = @import("std");

const screen_width: i32 = 320;
const screen_height: i32 = 200;
// const scale_factor: f32 = 100.0;

var height_map: [*]u8 = undefined;
var color_map: [*]u8 = undefined;

pub fn main() !void {
    std.debug.print("Testing Zig Voxel Engine.\n", .{});

    var general_purpose_allocator = std.heap.GeneralPurposeAllocator(.{}){};
    const gpa = general_purpose_allocator.allocator();
    const args = try std.process.argsAlloc(gpa);
    defer std.process.argsFree(gpa, args);

    // Convert the argument slices into just pointers, like C expects
    const arg_pointers = try gpa.alloc(?[*:0]const u8, args.len);
    defer gpa.free(arg_pointers);
    for (args) |arg, index| {
        arg_pointers[index] = arg.ptr;
    }

    _ = doslike.dosmain(@intCast(c_int, args.len), arg_pointers.ptr);

    doslike.setvideomode(doslike.videomode_320x200);

    var palette: *[256 * 3]u8 = undefined;
    var map_width: i32 = undefined;
    var map_height: i32 = undefined;
    var pal_count: i32 = undefined;

    color_map = doslike.loadgif("maps/gif/map0.color.gif", map_width, map_height, pal_count, palette);
    height_map = doslike.loadgif("maps/gif/map0.height.gif", null, null, null, null);

    var i: u8 = 0;
    while (i < pal_count) : (i += 1) {
        doslike.setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
    }

    // Set the first color to a specific color for the background
    // since the first color of the palette is the background color in DOS-Like
    doslike.setpal(0, 36, 36, 56);

    doslike.setdoublebuffer(1);

    var framebuffer = doslike.screenbuffer();

    while (doslike.shuttingdown() != 1) {
        doslike.waitvbl();
        doslike.clearscreen();

        framebuffer = doslike.swapbuffers();

        if (doslike.keystate(doslike.KEY_ESCAPE) == 1) {
            break;
        }
    }
}
