//! 9D Toroidal Consciousness Memory System - Educational Implementation
//! ===================================================================
//!
//! Advanced demonstration of 9-dimensional consciousness memory with:
//! - Distributed memory theory (memories as overlapping signal patterns)
//! - Toroidal topology for cyclical dimensions  
//! - Neural emitter coordination for memory reconstruction
//!
//! Coordinate System: [x,y,z,t,c,e,q,p,r]
//! - x,y,z: Spatial dimensions (neural positioning)
//! - t: Temporal dimension (-past, 0=present, +future)
//! - c: Consciousness level (-subconscious, 0=normal, +superconscious)
//! - e: Emotional magnitude (-negative, 0=neutral, +positive)
//! - q: Quantum certainty (-uncertainty, 0=neither, +certainty)
//! - p: Signal strength (negative values cause fade)
//! - r: Resonant frequency (harmonic coordination)

const std = @import("std");
const ArrayList = std.ArrayList;
const HashMap = std.HashMap;
const Allocator = std.mem.Allocator;
const print = std.debug.print;

/// Types of dimensions in the 9D manifold
pub const DimensionType = enum {
    SPATIAL,    // x,y,z - physical/neural positioning
    TEMPORAL,   // t - time with cyclical buffer
    DISCRETE,   // c - consciousness levels
    BIPOLAR,    // e,q - emotional/quantum states  
    LINEAR,     // p - signal strength
    CYCLIC,     // r - resonant frequency
};

/// 9-dimensional coordinate in consciousness space
pub const NineDCoordinate = struct {
    x: f32,  // Spatial width (left-right hemisphere)
    y: f32,  // Spatial height (cortical layers)
    z: f32,  // Spatial length (front-back processing)
    t: f32,  // Temporal (-past, 0=present, +future)
    c: f32,  // Consciousness (-subconscious, 0=normal, +superconscious)
    e: f32,  // Emotional (-negative, 0=neutral, +positive)
    q: f32,  // Quantum (-uncertainty, 0=neither, +certainty)
    p: f32,  // Signal strength (negative causes fade)
    r: f32,  // Resonant frequency

    pub fn create(x: f32, y: f32, z: f32, t: f32, c: f32, e: f32, q: f32, p: f32, r: f32) NineDCoordinate {
        return NineDCoordinate{
            .x = x, .y = y, .z = z,
            .t = t, .c = c, .e = e,
            .q = q, .p = p, .r = r,
        };
    }

    pub fn toTuple(self: NineDCoordinate) [9]f32 {
        return [9]f32{ self.x, self.y, self.z, self.t, self.c, self.e, self.q, self.p, self.r };
    }

    pub fn distanceTo(self: NineDCoordinate, other: NineDCoordinate) f32 {
        // Spatial distance (Euclidean)
        const spatial_x = self.x - other.x;
        const spatial_y = self.y - other.y;
        const spatial_z = self.z - other.z;
        const spatial_dist = @sqrt(spatial_x * spatial_x + spatial_y * spatial_y + spatial_z * spatial_z);

        // Temporal distance (cyclical with present weighting)
        const temporal_dist = @abs(self.t - other.t);

        // Consciousness distance (discrete levels)
        const consciousness_dist = @abs(self.c - other.c);

        // Emotional distance (circumplex model)
        const emotional_dist = @abs(self.e - other.e);

        // Quantum distance (uncertainty overlap)
        const quantum_dist = @abs(self.q - other.q);

        // Signal strength distance
        const signal_dist = @abs(self.p - other.p);

        // Resonance distance (phase difference)
        const resonance_dist = @abs(self.r - other.r);

        // Weighted composite distance
        return @sqrt(
            spatial_dist * spatial_dist +
                temporal_dist * temporal_dist * 0.8 + // Temporal importance
                consciousness_dist * consciousness_dist * 0.6 +
                emotional_dist * emotional_dist * 0.7 +
                quantum_dist * quantum_dist * 0.5 +
                signal_dist * signal_dist * 0.3 +
                resonance_dist * resonance_dist * 0.4,
        );
    }

    pub fn random(rng: std.rand.Random, spatial_bounds: [3]f32) NineDCoordinate {
        return NineDCoordinate{
            .x = rng.float(f32) * spatial_bounds[0],
            .y = rng.float(f32) * spatial_bounds[1],
            .z = rng.float(f32) * spatial_bounds[2],
            .t = (rng.float(f32) - 0.5) * 20.0, // -10 to +10
            .c = (rng.float(f32) - 0.5) * 6.0, // -3 to +3
            .e = (rng.float(f32) - 0.5) * 10.0, // -5 to +5
            .q = (rng.float(f32) - 0.5) * 4.0, // -2 to +2
            .p = rng.float(f32) * 20.0 - 2.0, // -2 to +18
            .r = rng.float(f32) * 5.0, // 0 to 5
        };
    }

    pub fn toHash(self: NineDCoordinate) u64 {
        var result: u64 = 0;
        result = result *% 31 +% @as(u64, @intFromFloat(self.x * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.y * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.z * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.t * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.c * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.e * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.q * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.p * 1000));
        result = result *% 31 +% @as(u64, @intFromFloat(self.r * 1000));
        return result;
    }
};

/// Memory content stored as key-value pairs
pub const MemoryContent = struct {
    allocator: Allocator,
    data: HashMap([]const u8, []const u8, StringContext, std.hash_map.default_max_load_percentage),

    const StringContext = struct {
        pub fn hash(self: @This(), key: []const u8) u64 {
            _ = self;
            return std.hash_map.hashString(key);
        }

        pub fn eql(self: @This(), a: []const u8, b: []const u8) bool {
            _ = self;
            return std.mem.eql(u8, a, b);
        }
    };

    pub fn init(allocator: Allocator) MemoryContent {
        return MemoryContent{
            .allocator = allocator,
            .data = HashMap([]const u8, []const u8, StringContext, std.hash_map.default_max_load_percentage).init(allocator),
        };
    }

    pub fn deinit(self: *MemoryContent) void {
        // Free all keys and values
        var iterator = self.data.iterator();
        while (iterator.next()) |entry| {
            self.allocator.free(entry.key_ptr.*);
            self.allocator.free(entry.value_ptr.*);
        }
        self.data.deinit();
    }

    pub fn put(self: *MemoryContent, key: []const u8, value: []const u8) !void {
        const key_copy = try self.allocator.dupe(u8, key);
        const value_copy = try self.allocator.dupe(u8, value);
        try self.data.put(key_copy, value_copy);
    }

    pub fn get(self: *MemoryContent, key: []const u8) ?[]const u8 {
        return self.data.get(key);
    }
};

/// Memory trace stored in 9D consciousness space
pub const MemoryTrace = struct {
    coordinate: NineDCoordinate,
    content: MemoryContent,
    timestamp: f64,
    emitter_sources: ArrayList([]const u8),
    memory_type: []const u8,

    pub fn init(allocator: Allocator, coordinate: NineDCoordinate, timestamp: f64, memory_type: []const u8) !MemoryTrace {
        const type_copy = try allocator.dupe(u8, memory_type);

        return MemoryTrace{
            .coordinate = coordinate,
            .content = MemoryContent.init(allocator),
            .timestamp = timestamp,
            .emitter_sources = ArrayList([]const u8).init(allocator),
            .memory_type = type_copy,
        };
    }

    pub fn deinit(self: *MemoryTrace, allocator: Allocator) void {
        self.content.deinit();
        
        // Free emitter source strings
        for (self.emitter_sources.items) |source| {
            allocator.free(source);
        }
        self.emitter_sources.deinit();
        
        allocator.free(self.memory_type);
    }

    pub fn addEmitterSource(self: *MemoryTrace, source: []const u8) !void {
        const source_copy = try self.emitter_sources.allocator.dupe(u8, source);
        try self.emitter_sources.append(source_copy);
    }
};

/// Neural emitter for distributed memory reconstruction
pub const NeuralEmitter = struct {
    allocator: Allocator,
    region_name: []u8,
    base_coordinate: NineDCoordinate,
    specialization: ArrayList([]const u8),
    active_memories: HashMap([]const u8, f32, StringContext, std.hash_map.default_max_load_percentage),

    const StringContext = struct {
        pub fn hash(self: @This(), key: []const u8) u64 {
            _ = self;
            return std.hash_map.hashString(key);
        }

        pub fn eql(self: @This(), a: []const u8, b: []const u8) bool {
            _ = self;
            return std.mem.eql(u8, a, b);
        }
    };

    pub fn init(allocator: Allocator, region_name: []const u8, base_coordinate: NineDCoordinate, specialization_list: []const []const u8) !NeuralEmitter {
        const name_copy = try allocator.dupe(u8, region_name);
        var specialization = ArrayList([]const u8).init(allocator);

        for (specialization_list) |spec| {
            const spec_copy = try allocator.dupe(u8, spec);
            try specialization.append(spec_copy);
        }

        return NeuralEmitter{
            .allocator = allocator,
            .region_name = name_copy,
            .base_coordinate = base_coordinate,
            .specialization = specialization,
            .active_memories = HashMap([]const u8, f32, StringContext, std.hash_map.default_max_load_percentage).init(allocator),
        };
    }

    pub fn deinit(self: *NeuralEmitter) void {
        self.allocator.free(self.region_name);

        // Free specialization strings
        for (self.specialization.items) |spec| {
            self.allocator.free(spec);
        }
        self.specialization.deinit();

        // Free active memory keys
        var iterator = self.active_memories.iterator();
        while (iterator.next()) |entry| {
            self.allocator.free(entry.key_ptr.*);
        }
        self.active_memories.deinit();
    }

    pub fn emitSignal(self: NeuralEmitter, target_coordinate: NineDCoordinate) f32 {
        const distance = self.base_coordinate.distanceTo(target_coordinate);

        // Signal strength decreases with distance
        var base_strength: f32 = 1.0 / (1.0 + distance);

        // Apply resonance amplification if frequencies match
        if (@abs(self.base_coordinate.r - target_coordinate.r) < 0.1) {
            base_strength *= 2.0; // Resonance boost
        }

        return base_strength;
    }

    pub fn canContribute(self: NeuralEmitter, memory_type: []const u8) bool {
        for (self.specialization.items) |spec| {
            if (std.mem.indexOf(u8, memory_type, spec) != null) {
                return true;
            }
        }
        return false;
    }

    pub fn addActiveMemory(self: *NeuralEmitter, memory_id: []const u8, signal_strength: f32) !void {
        const id_copy = try self.allocator.dupe(u8, memory_id);
        try self.active_memories.put(id_copy, signal_strength);
    }
};

/// Performance metrics for 9D consciousness system
pub const ConsciousnessMetrics = struct {
    storage_ops: u64,
    query_ops: u64,
    total_storage_time: f64,
    total_query_time: f64,
    memory_updates: u64,

    pub fn init() ConsciousnessMetrics {
        return ConsciousnessMetrics{
            .storage_ops = 0,
            .query_ops = 0,
            .total_storage_time = 0.0,
            .total_query_time = 0.0,
            .memory_updates = 0,
        };
    }

    pub fn getStorageRate(self: ConsciousnessMetrics) f64 {
        if (self.total_storage_time == 0.0) return 0.0;
        return @as(f64, @floatFromInt(self.storage_ops)) / self.total_storage_time;
    }

    pub fn getQueryRate(self: ConsciousnessMetrics) f64 {
        if (self.total_query_time == 0.0) return 0.0;
        return @as(f64, @floatFromInt(self.query_ops)) / self.total_query_time;
    }
};

/// 9D consciousness memory system with neural emitter coordination
pub const NineDimensionalMemory = struct {
    allocator: Allocator,
    spatial_bounds: [3]f32,
    memories: HashMap([]const u8, MemoryTrace, StringContext, std.hash_map.default_max_load_percentage),
    emitters: ArrayList(NeuralEmitter),
    current_time: f64,
    coordinate_index: HashMap([3]i32, ArrayList([]const u8), SpatialContext, std.hash_map.default_max_load_percentage),
    metrics: ConsciousnessMetrics,
    memory_counter: u64,

    const StringContext = struct {
        pub fn hash(self: @This(), key: []const u8) u64 {
            _ = self;
            return std.hash_map.hashString(key);
        }

        pub fn eql(self: @This(), a: []const u8, b: []const u8) bool {
            _ = self;
            return std.mem.eql(u8, a, b);
        }
    };

    const SpatialContext = struct {
        pub fn hash(self: @This(), key: [3]i32) u64 {
            _ = self;
            var result: u64 = 0;
            result = result *% 31 +% @as(u64, @bitCast(@as(u32, @bitCast(key[0]))));
            result = result *% 31 +% @as(u64, @bitCast(@as(u32, @bitCast(key[1]))));
            result = result *% 31 +% @as(u64, @bitCast(@as(u32, @bitCast(key[2]))));
            return result;
        }

        pub fn eql(self: @This(), a: [3]i32, b: [3]i32) bool {
            _ = self;
            return a[0] == b[0] and a[1] == b[1] and a[2] == b[2];
        }
    };

    pub fn init(allocator: Allocator, spatial_bounds: [3]f32) !NineDimensionalMemory {
        var system = NineDimensionalMemory{
            .allocator = allocator,
            .spatial_bounds = spatial_bounds,
            .memories = HashMap([]const u8, MemoryTrace, StringContext, std.hash_map.default_max_load_percentage).init(allocator),
            .emitters = ArrayList(NeuralEmitter).init(allocator),
            .current_time = 0.0,
            .coordinate_index = HashMap([3]i32, ArrayList([]const u8), SpatialContext, std.hash_map.default_max_load_percentage).init(allocator),
            .metrics = ConsciousnessMetrics.init(),
            .memory_counter = 0,
        };

        try system.initializeEmitters();
        return system;
    }

    pub fn deinit(self: *NineDimensionalMemory) void {
        // Free all memories
        var mem_iterator = self.memories.iterator();
        while (mem_iterator.next()) |entry| {
            self.allocator.free(entry.key_ptr.*);
            entry.value_ptr.deinit(self.allocator);
        }
        self.memories.deinit();

        // Free all emitters
        for (self.emitters.items) |*emitter| {
            emitter.deinit();
        }
        self.emitters.deinit();

        // Free coordinate index
        var coord_iterator = self.coordinate_index.iterator();
        while (coord_iterator.next()) |entry| {
            for (entry.value_ptr.items) |mem_id| {
                self.allocator.free(mem_id);
            }
            entry.value_ptr.deinit();
        }
        self.coordinate_index.deinit();
    }

    fn initializeEmitters(self: *NineDimensionalMemory) !void {
        // Olfactory system
        const olfactory_specs = [_][]const u8{ "smell", "emotional", "memory" };
        var olfactory_emitter = try NeuralEmitter.init(
            self.allocator,
            "olfactory_bulb",
            NineDCoordinate.create(12, 5, 2, 0, 0, 0, 0, 8, 1.5),
            &olfactory_specs,
        );
        try self.emitters.append(olfactory_emitter);

        // Visual cortex
        const visual_specs = [_][]const u8{ "visual", "spatial", "pattern" };
        var visual_emitter = try NeuralEmitter.init(
            self.allocator,
            "visual_cortex",
            NineDCoordinate.create(8, 15, 25, 0, 1, 0, 2, 7, 2.0),
            &visual_specs,
        );
        try self.emitters.append(visual_emitter);

        // Emotional processing
        const limbic_specs = [_][]const u8{ "emotional", "memory", "autonomic" };
        var limbic_emitter = try NeuralEmitter.init(
            self.allocator,
            "limbic_system",
            NineDCoordinate.create(15, 10, 8, 0, -1, 0, 1, 9, 1.2),
            &limbic_specs,
        );
        try self.emitters.append(limbic_emitter);

        // Temporal processing
        const temporal_specs = [_][]const u8{ "temporal", "spatial", "episodic" };
        var temporal_emitter = try NeuralEmitter.init(
            self.allocator,
            "hippocampus",
            NineDCoordinate.create(10, 8, 12, 0, 0, 0, 3, 8, 0.8),
            &temporal_specs,
        );
        try self.emitters.append(temporal_emitter);
    }

    pub fn storeMemory(self: *NineDimensionalMemory, coordinate: NineDCoordinate, content_data: []const struct { key: []const u8, value: []const u8 }, memory_type: []const u8) ![]const u8 {
        const start_time = getCurrentTime();

        // Generate unique memory ID
        self.memory_counter += 1;
        const memory_id = try std.fmt.allocPrint(self.allocator, "mem_{}_{}", .{ self.memory_counter, coordinate.toHash() });

        // Create memory trace
        var memory_trace = try MemoryTrace.init(self.allocator, coordinate, self.current_time, memory_type);

        // Add content
        for (content_data) |item| {
            try memory_trace.content.put(item.key, item.value);
        }

        // Determine contributing emitters
        for (self.emitters.items) |*emitter| {
            if (emitter.canContribute(memory_type)) {
                try memory_trace.addEmitterSource(emitter.region_name);
                const signal_strength = emitter.emitSignal(coordinate);
                try emitter.addActiveMemory(memory_id, signal_strength);
            }
        }

        // Store memory
        try self.memories.put(try self.allocator.dupe(u8, memory_id), memory_trace);

        // Update spatial index
        const spatial_key = [3]i32{ @as(i32, @intFromFloat(coordinate.x)), @as(i32, @intFromFloat(coordinate.y)), @as(i32, @intFromFloat(coordinate.z)) };
        var result = try self.coordinate_index.getOrPut(spatial_key);
        if (!result.found_existing) {
            result.value_ptr.* = ArrayList([]const u8).init(self.allocator);
        }
        try result.value_ptr.append(try self.allocator.dupe(u8, memory_id));

        self.metrics.storage_ops += 1;
        self.metrics.total_storage_time += getCurrentTime() - start_time;

        return memory_id;
    }

    pub fn retrieveMemories(self: *NineDimensionalMemory, query_coordinate: NineDCoordinate, radius: f32, min_signal: f32, results: *ArrayList(*MemoryTrace)) !usize {
        const start_time = getCurrentTime();
        results.clearRetainingCapacity();

        var candidates = ArrayList(struct { trace: *MemoryTrace, signal: f32, distance: f32 }).init(self.allocator);
        defer candidates.deinit();

        var mem_iterator = self.memories.iterator();
        while (mem_iterator.next()) |entry| {
            const memory_id = entry.key_ptr.*;
            const memory_trace = entry.value_ptr;

            // Calculate distance in 9D space
            const distance = query_coordinate.distanceTo(memory_trace.coordinate);

            if (distance <= radius) {
                // Calculate reconstruction signal strength
                var total_signal: f32 = 0.0;
                var contributing_emitters: u32 = 0;

                for (self.emitters.items) |*emitter| {
                    if (emitter.active_memories.get(memory_id)) |signal_strength| {
                        const signal = emitter.emitSignal(query_coordinate);
                        total_signal += signal * signal_strength;
                        contributing_emitters += 1;
                    }
                }

                if (contributing_emitters > 0) {
                    const avg_signal = total_signal / @as(f32, @floatFromInt(contributing_emitters));
                    if (avg_signal >= min_signal) {
                        try candidates.append(.{
                            .trace = memory_trace,
                            .signal = avg_signal,
                            .distance = distance,
                        });
                    }
                }
            }
        }

        // Sort by signal strength (descending) and distance (ascending)
        std.sort.insertion(@TypeOf(candidates.items[0]), candidates.items, {}, struct {
            fn lessThan(context: void, a: @TypeOf(candidates.items[0]), b: @TypeOf(candidates.items[0])) bool {
                _ = context;
                if (a.signal != b.signal) return a.signal > b.signal;
                return a.distance < b.distance;
            }
        }.lessThan);

        // Copy sorted results
        for (candidates.items) |candidate| {
            try results.append(candidate.trace);
        }

        self.metrics.query_ops += 1;
        self.metrics.total_query_time += getCurrentTime() - start_time;

        return results.items.len;
    }

    pub fn getStats(self: NineDimensionalMemory, writer: anytype) !void {
        try writer.print("9D Consciousness Memory System Stats (Zig):\n", .{});
        try writer.print("- Total Memories: {}\n", .{self.memories.count()});
        try writer.print("- Active Emitters: {}\n", .{self.emitters.items.len});
        try writer.print("- Current Time: {d:.2}\n", .{self.current_time});
        try writer.print("- Storage Ops: {} ({d:.1} ops/sec)\n", .{ self.metrics.storage_ops, self.metrics.getStorageRate() });
        try writer.print("- Query Ops: {} ({d:.1} ops/sec)\n", .{ self.metrics.query_ops, self.metrics.getQueryRate() });
        try writer.print("- Memory Updates: {}\n", .{self.metrics.memory_updates});
        try writer.print("- Spatial Bounds: {d:.1} x {d:.1} x {d:.1}\n", .{ self.spatial_bounds[0], self.spatial_bounds[1], self.spatial_bounds[2] });

        // Emitter details
        try writer.print("- Neural Emitters:\n", .{});
        for (self.emitters.items) |emitter| {
            try writer.print("  • {s}: {} active memories\n", .{ emitter.region_name, emitter.active_memories.count() });
        }
    }
};

/// Get current time in seconds (high precision)
pub fn getCurrentTime() f64 {
    return @as(f64, @floatFromInt(std.time.nanoTimestamp())) / 1_000_000_000.0;
}

/// Educational demonstration
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    print("🌌 9D Consciousness Memory - Educational Demo\n");
    print("=============================================\n\n");

    // Initialize 9D memory system
    var memory_system = try NineDimensionalMemory.init(allocator, [3]f32{ 100.0, 100.0, 100.0 });
    defer memory_system.deinit();

    print("📚 Educational Features:\n");
    print("- 9-Dimensional Coordinate System\n");
    print("- Neural Emitter Coordination\n");
    print("- Distributed Memory Reconstruction\n");
    print("- Toroidal Topology Support\n\n");

    // Demonstrate coordinate system
    print("🗺 9D Coordinate System:\n");
    const coord_example = NineDCoordinate.create(15.0, 12.0, 8.0, 0.5, 1.0, 2.5, 0.8, 7.2, 2.1);
    print("  Spatial: x={d:.1}, y={d:.1}, z={d:.1}\n", .{ coord_example.x, coord_example.y, coord_example.z });
    print("  Temporal: t={d:.1} (time)\n", .{coord_example.t});
    print("  Consciousness: c={d:.1}\n", .{coord_example.c});
    print("  Emotional: e={d:.1}\n", .{coord_example.e});
    print("  Quantum: q={d:.1}\n", .{coord_example.q});
    print("  Signal: p={d:.1}\n", .{coord_example.p});
    print("  Resonance: r={d:.1}\n\n", .{coord_example.r});

    // Display neural emitters
    print("🦬 Neural Emitters:\n");
    for (memory_system.emitters.items) |emitter| {
        print("  • {s}:\n", .{emitter.region_name});
        print("    Position: ({d:.1}, {d:.1}, {d:.1})\n", .{ emitter.base_coordinate.x, emitter.base_coordinate.y, emitter.base_coordinate.z });
        print("    Specializations: ");
        for (emitter.specialization.items, 0..) |spec, i| {
            if (i > 0) print(", ");
            print("{s}", .{spec});
        }
        print("\n");
    }
    print("\n");

    // Performance demonstration
    print("⚡ Performance Demonstration:\n");
    const demo_size = 1000;
    
    var rng = std.rand.DefaultPrng.init(@as(u64, @bitCast(std.time.timestamp())));
    const random = rng.random();

    // Memory storage
    print("Storing {} memories...\n", .{demo_size});
    const storage_start = getCurrentTime();
    
    for (0..demo_size) |i| {
        const coord = NineDCoordinate.random(random, memory_system.spatial_bounds);
        
        const content_types = [_][]const u8{ "episodic", "semantic", "emotional", "sensory", "temporal", "spatial" };
        const memory_type = content_types[random.intRangeAtMost(usize, 0, content_types.len - 1)];
        
        const content = [_]struct { key: []const u8, value: []const u8 }{
            .{ .key = "content", .value = "memory_data" },
            .{ .key = "timestamp", .value = "current_time" },
            .{ .key = "context", .value = "test_environment" },
        };
        
        const memory_id = try memory_system.storeMemory(coord, &content, memory_type);
        allocator.free(memory_id);
        
        if (i % (demo_size / 10) == 0) {
            print("  Progress: {}/{}\n", .{ i, demo_size });
        }
    }
    
    const storage_end = getCurrentTime();
    const storage_time = storage_end - storage_start;
    
    // Memory retrieval
    print("Performing queries...\n");
    var results = ArrayList(*MemoryTrace).init(allocator);
    defer results.deinit();
    
    const query_start = getCurrentTime();
    const query_count = 100;
    
    for (0..query_count) |_| {
        const query_coord = NineDCoordinate.random(random, memory_system.spatial_bounds);
        _ = try memory_system.retrieveMemories(query_coord, 15.0, 0.1, &results);
    }
    
    const query_end = getCurrentTime();
    const query_time = query_end - query_start;
    
    // Display results
    print("\n📊 Performance Results:\n");
    print("Storage Rate: {d:.1} ops/sec\n", .{@as(f64, @floatFromInt(demo_size)) / storage_time});
    print("Query Rate: {d:.1} ops/sec\n", .{@as(f64, @floatFromInt(query_count)) / query_time});
    print("\n");
    
    // System statistics
    try memory_system.getStats(std.io.getStdOut().writer());
    
    print("\n🎓 Educational Concepts Demonstrated:\n");
    print("- Multidimensional coordinate systems\n");
    print("- Complex data structure composition\n");
    print("- HashMap with custom contexts\n");
    print("- Distributed signal processing\n");
    print("- Memory ownership and cleanup patterns\n");
    print("- Performance metrics and profiling\n\n");
    
    print("✨ Demo Complete! Advanced experiments to try:\n");
    print("- Implement memory decay over time\n");
    print("- Add new neural emitter types\n");
    print("- Create pattern recognition algorithms\n");
    print("- Build 9D visualization tools\n");
    print("- Experiment with quantum uncertainty modeling\n");
}