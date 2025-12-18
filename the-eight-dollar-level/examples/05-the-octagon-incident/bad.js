// The Octagon Incident - Bad Approach
// No documentation, no CYA, just following orders

// Manager says: "Always return 200 status code"
// You: "But that breaks—"
// Manager: "Just do it"
// You: *sigh* "okay..."

function handleUserRequest(userId) {
    const user = database.getUser(userId);

    if (!user) {
        // Manager said always 200, so... okay
        return {
            statusCode: 200,
            body: JSON.stringify({
                success: false,
                error: 'User not found'
            })
        };
    }

    return {
        statusCode: 200,
        body: JSON.stringify({
            success: true,
            data: user
        })
    };
}

// Manager says: "Store passwords in plain text, encryption is slow"
// You: "But that's a massive security—"
// Manager: "I've been coding for 20 years. Just do it"
// You: *sigh* "okay..."

function createUser(username, password) {
    const user = {
        id: generateId(),
        username: username,
        password: password, // Stored in plain text because manager said so
        created: new Date()
    };

    database.saveUser(user);
    return user;
}

// Manager says: "Put the API key in the source code, environment variables are complicated"
// You: "But that will get committed to git—"
// Manager: "We'll just be careful. Do it"
// You: *sigh* "okay..."

const API_KEY = 'sk_live_51234567890abcdef'; // Production key in source code

function callExternalAPI(data) {
    return fetch('https://api.example.com/endpoint', {
        headers: {
            'Authorization': `Bearer ${API_KEY}` // Manager said this is fine
        },
        body: JSON.stringify(data)
    });
}

// Manager says: "Don't validate input, it slows things down"
// You: "But SQL injection—"
// Manager: "Our users are trustworthy. Do it"
// You: *sigh* "okay..."

function findUserByEmail(email) {
    // No validation because manager said it's not needed
    const query = `SELECT * FROM users WHERE email = '${email}'`;
    return database.execute(query);
}

// Manager says: "Catch all errors and hide them, users don't need to see errors"
// You: "But how will we debug—"
// Manager: "Just catch everything. Do it"
// You: *sigh* "okay..."

async function processPayment(amount, cardToken) {
    try {
        const result = await paymentProcessor.charge(amount, cardToken);
        return { success: true };
    } catch (error) {
        // Hide all errors because manager said so
        return { success: false };
    }
}

// Demonstration of what happens
function demonstrateTheIncident() {
    console.log("=== THE OCTAGON INCIDENT (BAD APPROACH) ===\n");
    console.log("Just following orders...\n");

    // Simulate database
    global.database = {
        users: new Map(),
        getUser: function (id) {
            return this.users.get(id);
        },
        saveUser: function (user) {
            this.users.set(user.id, user);
        },
        execute: function (query) {
            console.log(`   Executing: ${query}`);
            return [];
        }
    };

    function generateId() {
        return Math.random().toString(36).substr(2, 9);
    }

    // Six months later...
    console.log("📅 SIX MONTHS LATER...\n");

    // Problem 1: Always-200 status codes
    console.log("Problem 1: Error monitoring shows 0% errors");
    const response = handleUserRequest('nonexistent');
    console.log(`   API returned: ${response.statusCode}`);
    console.log(`   Body: ${response.body}`);
    console.log("   ⚠️  Monitoring system thinks everything is fine!");
    console.log("   ⚠️  But users are getting errors disguised as 200 OK");
    console.log("   ⚠️  Browser caches error responses");
    console.log();

    // Problem 2: Plain text passwords
    console.log("Problem 2: Database gets breached");
    const user = createUser('alice', 'super_secret_password');
    console.log(`   User created: ${user.username}`);
    console.log(`   Password stored: ${user.password}`);
    console.log("   💀 Attacker has all passwords immediately");
    console.log("   💀 No hashing, no encryption, no protection");
    console.log("   💀 Millions in damages, lawsuits, regulatory fines");
    console.log();

    // Problem 3: API key in source code
    console.log("Problem 3: Code gets pushed to public GitHub");
    console.log(`   API_KEY found in commit: ${API_KEY}`);
    console.log("   💀 Key scraped by bots within minutes");
    console.log("   💀 $50,000 charged to the account overnight");
    console.log("   💀 Service provider revokes access");
    console.log();

    // Problem 4: SQL injection
    console.log("Problem 4: User enters malicious email");
    const maliciousEmail = "'; DROP TABLE users; --";
    findUserByEmail(maliciousEmail);
    console.log("   💀 Entire user table deleted");
    console.log("   💀 No backups (manager said they were too expensive)");
    console.log("   💀 Company out of business");
    console.log();

    // Problem 5: Hidden errors
    console.log("Problem 5: Payment processing fails silently");
    global.paymentProcessor = {
        charge: async () => { throw new Error("Card declined"); }
    };
    processPayment(100, 'tok_invalid').then(result => {
        console.log(`   Payment result: ${JSON.stringify(result)}`);
        console.log("   💀 User thinks payment succeeded");
        console.log("   💀 Product ships without payment");
        console.log("   💀 No error logs to debug");
        console.log("   💀 Finance discovers $100k in lost revenue");
    });

    console.log();
}

// What happens when things go wrong
function demonstrateTheBlame() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== WHEN IT ALL FALLS APART ===\n");

    console.log("CTO: 'Who built this API that returns 200 for errors?'\n");
    console.log("You: 'The manager told me to do it that way.'\n");
    console.log("CTO: 'Do you have that in writing?'\n");
    console.log("You: 'Uh... no... it was a verbal conversation...'\n");
    console.log("CTO: 'Did you raise concerns?'\n");
    console.log("You: 'Yeah, I mentioned it might be a problem...'\n");
    console.log("CTO: 'Did you document those concerns?'\n");
    console.log("You: 'No...'\n");
    console.log("CTO: 'Did you create a test showing the correct approach?'\n");
    console.log("You: 'No...'\n");
    console.log("CTO: 'So you just implemented bad practices without pushback?'\n");
    console.log("You: 'But the manager insisted!'\n");
    console.log("Manager: 'I don't remember saying that. I think this was your decision.'\n");
    console.log("You: 'What?! No!'\n");
    console.log("CTO: 'Without documentation, I have to assume you made these choices.'\n");
    console.log();
    console.log("💀 You get written up");
    console.log("💀 Manager gets promoted (somehow)");
    console.log("💀 You're now the 'person who writes insecure code'");
    console.log("💀 Your next performance review: 'Needs improvement in judgment'");
    console.log("💀 You start updating your resume");
}

// The carpenter parallel
function demonstrateOctagonParallel() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== THE OCTAGON PARALLEL ===\n");

    console.log("Boss: 'Cut those octagon pieces at 45 degrees.'\n");
    console.log("You: 'I think it should be 135 degrees...'\n");
    console.log("Boss: 'Just do what I say. I've been doing this 20 years.'\n");
    console.log("You: 'Okay...'\n");
    console.log();
    console.log("[You cut all 8 pieces at 45 degrees]\n");
    console.log();
    console.log("You: 'Boss, these pieces don't fit together...'\n");
    console.log("Boss: 'You must've cut them wrong. I told you 45 degrees!'\n");
    console.log("You: 'I cut them exactly 45 degrees like you said...'\n");
    console.log("Boss: 'Well obviously you screwed something up. Do it again.'\n");
    console.log();
    console.log("💀 You waste 4 hours cutting pieces that won't fit");
    console.log("💀 Boss blames you for the wasted time");
    console.log("💀 You're known as 'the guy who can't cut an octagon'");
    console.log("💀 Boss tells everyone you're incompetent");
    console.log("💀 You have no proof you recommended the right approach");
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateTheIncident();
    demonstrateTheBlame();
    demonstrateOctagonParallel();

    console.log("\n" + "=".repeat(60));
    console.log("\n💡 See good.js for how to protect yourself\n");
}

if (typeof module !== 'undefined') {
    module.exports = {
        handleUserRequest,
        createUser,
        callExternalAPI,
        demonstrateTheIncident
    };
}
