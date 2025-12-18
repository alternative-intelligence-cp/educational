// Static Typing Sanity - Do the math on the ground

// Type definitions
/**
 * @typedef {Object} UserProfile
 * @property {string} name
 * @property {string} avatar
 */

/**
 * @typedef {Object} Post
 * @property {number} id
 * @property {string} title
 */

/**
 * @typedef {Object} User
 * @property {number} id
 * @property {UserProfile | null} profile
 * @property {number} age
 * @property {Post[]} posts
 */

/**
 * Fetch user with type safety via JSDoc
 * @param {number} userId 
 * @returns {Promise<User>}
 */
async function fetchUser(userId) {
    const response = await fetch(`/api/users/${userId}`);
    const data = await response.json();
    // In real TypeScript, this would be validated
    return data;
}

/**
 * Display user profile with compile-time safety
 * @param {User} user 
 * @returns {string}
 */
function displayUserProfile(user) {
    // Handle null profile (compiler would warn about this)
    if (!user.profile) {
        return "Profile not available";
    }

    const name = user.profile.name.toUpperCase();
    const age = user.age;
    const posts = user.posts.length;

    return `${name} (${age} years old) has ${posts} posts`;
}

// Demonstration of type safety benefits
function demonstrateTypeSafety() {
    console.log("=== ZERO LADDER CLIMBS ===\n");
    console.log("With types, compiler catches ALL errors before running:\n");

    // Valid user
    const validUser = {
        id: 1,
        profile: { name: "Alice", avatar: "avatar.jpg" },
        age: 25,
        posts: [
            { id: 1, title: "First Post" },
            { id: 2, title: "Second Post" }
        ]
    };

    console.log("✅ Valid user:");
    console.log("   " + displayUserProfile(validUser));
    console.log();

    // User with null profile (handled gracefully)
    const noProfileUser = {
        id: 2,
        profile: null,
        age: 30,
        posts: []
    };

    console.log("✅ Null profile (gracefully handled):");
    console.log("   " + displayUserProfile(noProfileUser));
    console.log();

    // These would be COMPILE ERRORS in TypeScript:
    console.log("🚫 COMPILER WOULD REJECT:");
    console.log();

    const errors = [
        {
            desc: "Missing profile field",
            code: "{ id: 1, age: 25, posts: [] }",
            error: "Property 'profile' is missing"
        },
        {
            desc: "Profile name is number",
            code: "{ profile: { name: 123 } }",
            error: "Type 'number' is not assignable to type 'string'"
        },
        {
            desc: "Age is string",
            code: "{ age: '25' }",
            error: "Type 'string' is not assignable to type 'number'"
        },
        {
            desc: "Posts is not array",
            code: "{ posts: 'invalid' }",
            error: "Type 'string' is not assignable to type 'Post[]'"
        },
        {
            desc: "Missing required fields",
            code: "{ id: 1 }",
            error: "Type '{ id: number }' is missing properties: profile, age, posts"
        }
    ];

    errors.forEach((err, i) => {
        console.log(`   ${i + 1}. ${err.desc}`);
        console.log(`      Code: ${err.code}`);
        console.log(`      Error: ${err.error}`);
        console.log();
    });

    console.log("=== ANALYSIS ===\n");
    console.log("Ladder climbs: 0 ✅");
    console.log("Errors caught: ALL (at compile time)");
    console.log("Time to working code: 5 minutes");
    console.log("Runtime errors: IMPOSSIBLE");
    console.log("Developer sanity: PRESERVED");
    console.log();
    console.log("✅ Compiler acts as safety net");
    console.log("✅ Can't even run bad code");
    console.log("✅ Like calculating rafter length before cutting");
    console.log("✅ Cut once, climb once, perfect fit");
}

// Order processing with type safety
/**
 * @typedef {Object} OrderItem
 * @property {number} price
 * @property {number} quantity
 */

/**
 * @typedef {Object} Address
 * @property {string} country
 * @property {string} state
 */

/**
 * @typedef {Object} Order
 * @property {OrderItem[]} items
 * @property {Address} address
 */

/**
 * @typedef {Object} OrderTotal
 * @property {number} subtotal
 * @property {number} shipping
 * @property {number} tax
 * @property {number} total
 */

/**
 * Process order with type safety
 * @param {Order} order 
 * @returns {OrderTotal}
 */
function processOrder(order) {
    const subtotal = order.items.reduce((sum, item) =>
        sum + (item.price * item.quantity), 0
    );

    const shipping = order.address.country === 'US' ? 5.99 : 12.99;

    const TAX_RATES = { 'CA': 0.0725, 'NY': 0.04, 'TX': 0.0625 };
    const tax = subtotal * (TAX_RATES[order.address.state] || 0);

    return {
        subtotal,
        shipping,
        tax,
        total: subtotal + shipping + tax
    };
}

function orderProcessingExample() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== ORDER PROCESSING WITH TYPES ===\n");

    const validOrder = {
        items: [
            { price: 10.00, quantity: 2 },
            { price: 15.00, quantity: 1 }
        ],
        address: { country: 'US', state: 'CA' }
    };

    const result = processOrder(validOrder);
    console.log("Order total:", result);
    console.log();

    console.log("🚫 COMPILER WOULD REJECT:");
    console.log();
    console.log("   const badOrder1 = {");
    console.log("       items: null,  // ❌ Type 'null' not assignable to 'OrderItem[]'");
    console.log("       address: { country: 'US', state: 'CA' }");
    console.log("   };");
    console.log();
    console.log("   const badOrder2 = {");
    console.log("       items: [{ price: 10, quantity: 2 }]");
    console.log("       // ❌ Property 'address' is missing");
    console.log("   };");
    console.log();
    console.log("   const badOrder3 = {");
    console.log("       items: [{ price: '10', quantity: 2 }],  // ❌ price must be number");
    console.log("       address: { country: 'US', state: 'CA' }");
    console.log("   };");
    console.log();
    console.log("✅ Can't compile broken code = Can't ship broken code");
}

// Comparison demo
function runComparison() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== DIRECT COMPARISON ===\n");

    console.log("WITHOUT TYPES:");
    console.log("  - Cut rafter by eye");
    console.log("  - Climb ladder");
    console.log("  - Doesn't fit");
    console.log("  - Repeat 14 times");
    console.log("  - 2+ hours wasted");
    console.log();

    console.log("WITH TYPES:");
    console.log("  - Calculate length on ground");
    console.log("  - Cut once");
    console.log("  - Climb once");
    console.log("  - Perfect fit");
    console.log("  - 10 minutes total");
    console.log();

    console.log("🎯 Types = Math before cutting");
    console.log("🎯 No types = Trial and error");
    console.log();
    console.log("The ladder doesn't lie. Neither does the compiler.");
}

// Run all demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateTypeSafety();
    orderProcessingExample();
    runComparison();
}

if (typeof module !== 'undefined') {
    module.exports = {
        fetchUser,
        displayUserProfile,
        processOrder,
        demonstrateTypeSafety,
        orderProcessingExample
    };
}
