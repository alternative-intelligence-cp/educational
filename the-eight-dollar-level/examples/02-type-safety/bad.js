// The 14 Ladder Climbs - Dynamic Typing Hell

async function fetchUser(userId) {
    const response = await fetch(`/api/users/${userId}`);
    const data = await response.json();
    return data;  // What is data? Who knows!
}

function displayUserProfile(user) {
    // Hope and pray user has the fields we need
    const name = user.profile.name.toUpperCase();
    const age = user.age;
    const posts = user.posts.length;

    return `${name} (${age} years old) has ${posts} posts`;
}

// Simulate ladder climbs with progressive fixes
function demonstrateLadderClimbs() {
    console.log("=== THE 14 LADDER CLIMBS ===\n");
    console.log("Watch as we discover errors ONE AT A TIME at runtime...\n");

    // Test cases that progressively break
    const testCases = [
        {
            name: "Climb 1: Missing profile",
            data: { id: 1, age: 25, posts: [] },
            expectedError: "Cannot read property 'name' of undefined"
        },
        {
            name: "Climb 2: Null profile",
            data: { id: 1, profile: null, age: 25, posts: [] },
            expectedError: "Cannot read property 'name' of null"
        },
        {
            name: "Climb 3: Missing name",
            data: { id: 1, profile: {}, age: 25, posts: [] },
            expectedError: "Cannot read property 'toUpperCase' of undefined"
        },
        {
            name: "Climb 4: Name is number",
            data: { id: 1, profile: { name: 123 }, age: 25, posts: [] },
            expectedError: "user.profile.name.toUpperCase is not a function"
        },
        {
            name: "Climb 5: Missing age",
            data: { id: 1, profile: { name: "Alice" }, posts: [] },
            expectedError: "age is undefined"
        },
        {
            name: "Climb 6: Age is string",
            data: { id: 1, profile: { name: "Alice" }, age: "25", posts: [] },
            expectedError: "String used where number expected"
        },
        {
            name: "Climb 7: Missing posts",
            data: { id: 1, profile: { name: "Alice" }, age: 25 },
            expectedError: "Cannot read property 'length' of undefined"
        },
        {
            name: "Climb 8: Null posts",
            data: { id: 1, profile: { name: "Alice" }, age: 25, posts: null },
            expectedError: "Cannot read property 'length' of null"
        },
        {
            name: "Climb 9: Posts is not array",
            data: { id: 1, profile: { name: "Alice" }, age: 25, posts: "invalid" },
            expectedError: "posts.length returns wrong type"
        },
        {
            name: "Climb 10-14: Various edge cases",
            data: {},
            expectedError: "Multiple cascading failures..."
        }
    ];

    let climbCount = 0;

    for (const testCase of testCases) {
        climbCount++;
        console.log(`🪜 LADDER CLIMB ${climbCount}: ${testCase.name}`);

        try {
            const result = displayUserProfile(testCase.data);
            if (climbCount === testCases.length) {
                console.log(`   ✅ Finally works: ${result}`);
            } else {
                console.log(`   ⚠️  Appears to work but might have issues: ${result}`);
            }
        } catch (error) {
            console.log(`   ❌ ERROR: ${error.message}`);
            console.log(`   💀 Now you debug, fix, and try again...`);
        }

        console.log();
    }

    console.log("=== ANALYSIS ===\n");
    console.log(`Total ladder climbs: ${climbCount}`);
    console.log(`Time wasted: ~${climbCount * 10} minutes (debugging each error)`);
    console.log(`Developer sanity: DESTROYED`);
    console.log(`Production incidents: MULTIPLE`);
    console.log();
    console.log("💀 Each error discovered at RUNTIME");
    console.log("💀 Each fix reveals NEW error");
    console.log("💀 Like cutting rafter, climbing, finding it wrong, repeat");
    console.log();
    console.log("Could have been avoided with: TYPES");
}

// The defensive programming approach (still bad)
function displayUserProfileDefensive(user) {
    // Add ALL the checks...
    if (!user) return "User is null";
    if (!user.profile) return "Profile missing";
    if (!user.profile.name) return "Name missing";
    if (typeof user.profile.name !== 'string') return "Name is not string";
    if (typeof user.age !== 'number') return "Age is not number";
    if (!user.posts) return "Posts missing";
    if (!Array.isArray(user.posts)) return "Posts is not array";

    const name = user.profile.name.toUpperCase();
    const age = user.age;
    const posts = user.posts.length;

    return `${name} (${age} years old) has ${posts} posts`;
}

console.log("DEFENSIVE APPROACH:");
console.log("- Cluttered with checks");
console.log("- Still no compile-time verification");
console.log("- Easy to miss cases");
console.log("- Doesn't prevent bad data from entering");
console.log();

// Real-world cascade example
function orderProcessingExample() {
    console.log("=== REAL-WORLD CASCADE ===\n");
    console.log("Order processing without types:\n");

    function processOrder(order) {
        const total = order.items.reduce((sum, item) =>
            sum + (item.price * item.quantity), 0
        );

        const shipping = order.address.country === 'US' ? 5.99 : 12.99;
        const tax = total * (order.address.state === 'CA' ? 0.0725 : 0);

        return { total, shipping, tax };
    }

    try {
        // This works
        const order1 = {
            items: [{ price: 10, quantity: 2 }],
            address: { country: 'US', state: 'CA' }
        };
        console.log("Order 1:", processOrder(order1));

        // This explodes
        const order2 = {
            items: null,  // API returned null!
            address: { country: 'US', state: 'CA' }
        };
        console.log("Order 2:", processOrder(order2));
    } catch (error) {
        console.log("❌ ERROR:", error.message);
        console.log("🪜 Ladder climb 1: Discover items can be null");
        console.log("🪜 Ladder climb 2: Add null check");
        console.log("🪜 Ladder climb 3: Discover address can be missing");
        console.log("🪜 ...and so on");
    }
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateLadderClimbs();
    console.log("\n" + "=".repeat(60) + "\n");
    orderProcessingExample();

    console.log("\n" + "=".repeat(60));
    console.log("\n🎯 THE LESSON:");
    console.log("   Types = Doing the math on the ground");
    console.log("   No types = 14 ladder climbs");
    console.log();
    console.log("   The ladder doesn't lie. Neither does the compiler.");
}

if (typeof module !== 'undefined') {
    module.exports = {
        fetchUser,
        displayUserProfile,
        displayUserProfileDefensive,
        demonstrateLadderClimbs
    };
}
