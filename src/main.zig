const doslike = @cImport(@cInclude("dos.h"));
const std = @import("std");

const screen_width: i32 = 320;
const screen_height: i32 = 200;
// const scale_factor: f32 = 100.0;

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
}
