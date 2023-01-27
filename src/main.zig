const doslike = @cImport(@cInclude("dos.h"));
const std = @import("std");

const screen_width: i32 = 320;
const screen_height: i32 = 200;
// const scale_factor: f32 = 100.0;

pub fn main() !void {
    std.debug.print("Testing Zig Voxel Engine.\n", .{});
    doslike.setvideomode(doslike.videomode_320x200);
}
