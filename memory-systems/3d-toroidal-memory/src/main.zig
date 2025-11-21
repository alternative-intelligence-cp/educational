//! 3D Pentavalent Logic Core - Zig Implementation
//! =============================================
//!
//! High-performance Zig implementation of 3D cellular consciousness
//! with pentavalent logic states and cellular memory management.

const std = @import("std");
const testing = std.testing;
const Allocator = std.mem.Allocator;
const ArrayList = std.ArrayList;
const HashMap = std.HashMap;
const print = std.debug.print;

/// Pentavalent Logic States
/// Five-state logic system for cellular consciousness
pub const LogicState = enum(i8) {
    negative = -1, // False, inhibited, past, bad
    zero = 0,      // None, neutral, origin, off
    positive = 1,  // True, excitatory, future, good
    both = 2,      // Superposition, paradox, both states
    unknown = 3,   // Wildcard, entangled, latent, uncertain

    pub fn toString(self: LogicState) []const u8 {
        return switch (self) {
            .negative => "NEGATIVE",
            .zero => "ZERO",
            .positive => "POSITIVE",
            .both => "BOTH",
            .unknown => "UNKNOWN",
        };
    }

    pub fn fromString(str: []const u8) LogicState {
        if (std.mem.eql(u8, str, "NEGATIVE")) return .negative;
        if (std.mem.eql(u8, str, "ZERO")) return .zero;
        if (std.mem.eql(u8, str, "POSITIVE")) return .positive;
        if (std.mem.eql(u8, str, "BOTH")) return .both;
        if (std.mem.eql(u8, str, "UNKNOWN")) return .unknown;
        return .unknown;
    }

    pub fn random(rng: std.rand.Random) LogicState {
        const states = [_]LogicState{ .negative, .zero, .positive, .both, .unknown };
        return states[rng.intRangeAtMost(usize, 0, 4)];
    }
};

/// 3D Coordinate in pentavalent logic space
/// Represents position in 3D cellular consciousness topology
pub const ThreeDCoordinate = struct {
    a: LogicState, // Alpha - Alignment/Truth
    b: LogicState, // Beta - Bias/Valence
    c: LogicState, // Context - Consciousness/Visibility

    pub fn create(a: LogicState, b: LogicState, c: LogicState) ThreeDCoordinate {
        return ThreeDCoordinate{ .a = a, .b = b, .c = c };
    }

    pub fn distanceTo(self: ThreeDCoordinate, other: ThreeDCoordinate) f64 {
        const a_diff = @as(i32, @intFromEnum(self.a)) - @as(i32, @intFromEnum(other.a));
        const b_diff = @as(i32, @intFromEnum(self.b)) - @as(i32, @intFromEnum(other.b));
        const c_diff = @as(i32, @intFromEnum(self.c)) - @as(i32, @intFromEnum(other.c));
        
        const diff_a = if (a_diff < 0) -a_diff else a_diff;
        const diff_b = if (b_diff < 0) -b_diff else b_diff;
        const diff_c = if (c_diff < 0) -c_diff else c_diff;

        const sum = @as(f64, @floatFromInt(diff_a * diff_a + diff_b * diff_b + diff_c * diff_c));
        return @sqrt(sum);
    }

    pub fn random(rng: std.rand.Random) ThreeDCoordinate {
        return ThreeDCoordinate{
            .a = LogicState.random(rng),
            .b = LogicState.random(rng),
            .c = LogicState.random(rng),
        };
    }
};

/// Cellular Memory Engram
/// Individual memory pattern stored in cellular consciousness
pub const CellularEngram = struct {
    engram_id: [32]u8,
    coordinate: ThreeDCoordinate,
    logic_state: LogicState,
    content: []u8,
    creation_time: f64,
    last_access: f64,
    access_count: u32,
    is_active: bool,

    pub fn init(
        allocator: Allocator,
        coordinate: ThreeDCoordinate,
        logic_state: LogicState,
        content: []const u8,
        creation_time: f64,
    ) !CellularEngram {
        var engram_id: [32]u8 = undefined;
        generateEngramId(&engram_id);

        var owned_content = try allocator.dupe(u8, content);

        return CellularEngram{
            .engram_id = engram_id,
            .coordinate = coordinate,
            .logic_state = logic_state,
            .content = owned_content,
            .creation_time = creation_time,
            .last_access = creation_time,
            .access_count = 0,
            .is_active = true,
        };
    }

    pub fn deinit(self: *CellularEngram, allocator: Allocator) void {
        allocator.free(self.content);
        self.content = &[_]u8{};
    }

    fn generateEngramId(id_buffer: *[32]u8) void {
        const hex_chars = "0123456789abcdef";
        var rng = std.rand.DefaultPrng.init(@as(u64, @bitCast(std.time.timestamp())));
        const random = rng.random();

        for (id_buffer) |*char| {
            char.* = hex_chars[random.intRangeAtMost(usize, 0, 15)];
        }
    }
};

/// Performance metrics structure
pub const PerformanceMetrics = struct {
    storage_ops: u64 = 0,
    query_ops: u64 = 0,
    total_storage_time: f64 = 0.0,
    total_query_time: f64 = 0.0,

    pub fn getStorageRate(self: PerformanceMetrics) f64 {
        if (self.total_storage_time <= 0.0) return 0.0;
        return @as(f64, @floatFromInt(self.storage_ops)) / self.total_storage_time;
    }

    pub fn getQueryRate(self: PerformanceMetrics) f64 {
        if (self.total_query_time <= 0.0) return 0.0;
        return @as(f64, @floatFromInt(self.query_ops)) / self.total_query_time;
    }
};

/// Cellular Consciousness System
/// Main consciousness processing system
pub const CellularConsciousness = struct {
    allocator: Allocator,
    engrams: ArrayList(CellularEngram),
    current_time: f64,
    total_operations: u64,
    metrics: PerformanceMetrics,

    pub fn init(allocator: Allocator, initial_capacity: usize) !CellularConsciousness {
        var engrams = ArrayList(CellularEngram).init(allocator);
        try engrams.ensureTotalCapacity(initial_capacity);

        return CellularConsciousness{
            .allocator = allocator,
            .engrams = engrams,
            .current_time = getCurrentTime(),
            .total_operations = 0,
            .metrics = PerformanceMetrics{},
        };
    }

    pub fn deinit(self: *CellularConsciousness) void {
        // Free all engram content
        for (self.engrams.items) |*engram| {
            engram.deinit(self.allocator);
        }
        self.engrams.deinit();
    }

    pub fn storeEngram(
        self: *CellularConsciousness,
        coordinate: ThreeDCoordinate,
        content: []const u8,
        logic_state: LogicState,
    ) ![]const u8 {
        const start_time = getCurrentTime();

        const engram = try CellularEngram.init(
            self.allocator,
            coordinate,
            logic_state,
            content,
            start_time,
        );

        try self.engrams.append(engram);
        self.total_operations += 1;
        self.metrics.storage_ops += 1;

        const end_time = getCurrentTime();
        self.metrics.total_storage_time += (end_time - start_time);

        // Return reference to the stored ID
        const stored_engram = &self.engrams.items[self.engrams.items.len - 1];
        return stored_engram.engram_id[0..];
    }

    pub fn queryByState(
        self: *CellularConsciousness,
        target_state: LogicState,
        results: *ArrayList(*CellularEngram),
        max_results: usize,
    ) !usize {
        const start_time = getCurrentTime();
        results.clearRetainingCapacity();

        var found: usize = 0;
        for (self.engrams.items) |*engram| {
            if (!engram.is_active or found >= max_results) continue;

            // Check if any coordinate dimension matches target state
            if (engram.coordinate.a == target_state or
                engram.coordinate.b == target_state or
                engram.coordinate.c == target_state)
            {
                try results.append(engram);
                engram.last_access = getCurrentTime();
                engram.access_count += 1;
                found += 1;
            }
        }

        self.total_operations += 1;
        self.metrics.query_ops += 1;

        const end_time = getCurrentTime();
        self.metrics.total_query_time += (end_time - start_time);

        return found;
    }

    pub fn queryByProximity(
        self: *CellularConsciousness,
        center: ThreeDCoordinate,
        radius: f64,
        results: *ArrayList(*CellularEngram),
        max_results: usize,
    ) !usize {
        const start_time = getCurrentTime();
        results.clearRetainingCapacity();

        var found: usize = 0;
        for (self.engrams.items) |*engram| {
            if (!engram.is_active or found >= max_results) continue;

            const distance = center.distanceTo(engram.coordinate);
            if (distance <= radius) {
                try results.append(engram);
                engram.last_access = getCurrentTime();
                engram.access_count += 1;
                found += 1;
            }
        }

        self.total_operations += 1;
        self.metrics.query_ops += 1;

        const end_time = getCurrentTime();
        self.metrics.total_query_time += (end_time - start_time);

        return found;
    }

    pub fn getStats(self: CellularConsciousness, writer: anytype) !void {
        try writer.print("Cellular Consciousness Stats (Zig):\n", .{});
        try writer.print("- Total Engrams: {}/{}\n", .{ self.engrams.items.len, self.engrams.capacity });
        try writer.print("- Total Operations: {}\n", .{self.total_operations});
        try writer.print("- Storage Ops: {} ({d:.1} ops/sec)\n", .{ self.metrics.storage_ops, self.metrics.getStorageRate() });
        try writer.print("- Query Ops: {} ({d:.1} ops/sec)\n", .{ self.metrics.query_ops, self.metrics.getQueryRate() });
        try writer.print("- Storage Time: {d:.6} sec\n", .{self.metrics.total_storage_time});
        try writer.print("- Query Time: {d:.6} sec\n", .{self.metrics.total_query_time});
    }

    pub fn updateTime(self: *CellularConsciousness) void {
        self.current_time = getCurrentTime();
    }
};

/// Get current time in seconds (high precision)
fn getCurrentTime() f64 {
    return @as(f64, @floatFromInt(std.time.nanoTimestamp())) / 1_000_000_000.0;
}

// Educational Demo
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    print("🔮 3D Toroidal Memory - Educational Demo\n");
    print("========================================\n\n");

    // Initialize consciousness system
    var consciousness = try CellularConsciousness.init(allocator, 10000);
    defer consciousness.deinit();

    print("📚 Educational Features:\n");
    print("- Pentavalent Logic States (5 states)\n");
    print("- 3D Coordinate System\n");
    print("- Cellular Memory Engrams\n");
    print("- Spatial Proximity Queries\n\n");

    // Demonstrate logic states
    print("🧠 Pentavalent Logic States:\n");
    const states = [_]LogicState{ .negative, .zero, .positive, .both, .unknown };
    for (states) |state| {
        print("  • {s} ({})\n", .{ state.toString(), @intFromEnum(state) });
    }
    print("\n");

    // Performance demonstration
    print("⚡ Performance Demonstration:\n");
    const demo_size = 10000;
    
    var rng = std.rand.DefaultPrng.init(@as(u64, @bitCast(std.time.timestamp())));
    const random = rng.random();

    // Storage benchmark
    print("Storing {} engrams...\n", .{demo_size});
    const storage_start = getCurrentTime();
    
    for (0..demo_size) |i| {
        const coord = ThreeDCoordinate.random(random);
        const content = try std.fmt.allocPrint(allocator, "memory_{}", .{i});
        defer allocator.free(content);
        
        const logic_state = LogicState.random(random);
        _ = try consciousness.storeEngram(coord, content, logic_state);
    }
    
    const storage_end = getCurrentTime();
    const storage_time = storage_end - storage_start;
    
    // Query benchmark
    print("Performing queries...\n");
    var results = ArrayList(*CellularEngram).init(allocator);
    defer results.deinit();
    
    const query_start = getCurrentTime();
    
    for (states) |state| {
        _ = try consciousness.queryByState(state, &results, 100);
    }
    
    // Proximity queries
    for (0..100) |_| {
        const center = ThreeDCoordinate.random(random);
        _ = try consciousness.queryByProximity(center, 2.0, &results, 50);
    }
    
    const query_end = getCurrentTime();
    const query_time = query_end - query_start;
    
    // Display results
    print("\n📊 Performance Results:\n");
    print("Storage Rate: {d:.1} ops/sec\n", .{@as(f64, @floatFromInt(demo_size)) / storage_time});
    print("Query Rate: {d:.1} ops/sec\n", .{@as(f64, @floatFromInt(105)) / query_time});
    print("\n");
    
    // System statistics
    try consciousness.getStats(std.io.getStdOut().writer());
    
    print("\n🎓 Educational Concepts Demonstrated:\n");
    print("- Custom enum types with methods\n");
    print("- Struct composition and methods\n");
    print("- Dynamic arrays (ArrayList)\n");
    print("- Memory management with allocators\n");
    print("- Performance measurement and optimization\n");
    print("- Spatial data structure basics\n\n");
    
    print("✨ Demo Complete! Try modifying the code to:\n");
    print("- Add new logic states\n");
    print("- Implement spatial indexing\n");
    print("- Add persistence layer\n");
    print("- Create visualization tools\n");
}

// Tests
test "LogicState operations" {
    const state = LogicState.positive;
    try testing.expect(std.mem.eql(u8, state.toString(), "POSITIVE"));
    try testing.expect(LogicState.fromString("POSITIVE") == LogicState.positive);
}

test "ThreeDCoordinate distance calculation" {
    const coord1 = ThreeDCoordinate.create(.positive, .zero, .negative);
    const coord2 = ThreeDCoordinate.create(.negative, .positive, .unknown);
    
    const distance = coord1.distanceTo(coord2);
    try testing.expect(distance > 0.0);
}

test "CellularConsciousness basic operations" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var consciousness = try CellularConsciousness.init(allocator, 10);
    defer consciousness.deinit();

    // Test storage
    const coord = ThreeDCoordinate.create(.positive, .zero, .unknown);
    const content = "test_content";
    const id = try consciousness.storeEngram(coord, content, .positive);
    
    try testing.expect(id.len == 32);
    try testing.expect(consciousness.engrams.items.len == 1);

    // Test query
    var results = ArrayList(*CellularEngram).init(allocator);
    defer results.deinit();
    
    const found = try consciousness.queryByState(.positive, &results, 10);
    try testing.expect(found >= 1);
    try testing.expect(results.items.len >= 1);
}