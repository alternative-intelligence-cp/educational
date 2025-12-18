// The Octagon Incident - Good Approach
// Document everything, CYA mode activated

/**
 * ARCHITECTURAL DECISION RECORD #001
 * Date: 2025-12-12
 * Title: HTTP Status Codes
 * 
 * CONTEXT:
 * Building REST API for user management system.
 * Industry standard is to use appropriate HTTP status codes (404, 500, etc).
 * 
 * DECISION:
 * Always return HTTP 200 status code, even for errors.
 * 
 * DECIDED BY:
 * John Smith, Engineering Manager (john.smith@company.com)
 * Decision made in Slack thread: https://slack.com/archives/C123/p1702339200
 * 
 * RATIONALE (as stated by decision maker):
 * "To be innovative like teams at NodeConf"
 * "Simplifies client-side error handling"
 * 
 * CONCERNS RAISED BY ENGINEERING TEAM:
 * 1. Violates HTTP semantics (RFC 7231)
 * 2. Breaks browser caching strategies
 * 3. Prevents error monitoring tools from working
 * 4. Forces clients to parse every response body
 * 5. Violates REST principles
 * 6. Makes debugging harder
 * 
 * RECOMMENDATION FROM ENGINEERING:
 * Use standard HTTP status codes per RFC 7231:
 * - 200: Success
 * - 404: Not found
 * - 500: Server error
 * 
 * See test/proof-of-concept/status-codes-comparison.js for demonstration
 * 
 * MANAGEMENT OVERRIDE:
 * Engineering recommendation overridden by John Smith on 2025-12-12
 * 
 * STAKEHOLDERS NOTIFIED:
 * - john.smith@company.com (Engineering Manager)
 * - sarah.jones@company.com (CTO)
 * - tech-team@company.com (Engineering Team)
 * 
 * DOCUMENTED IN:
 * - Slack thread: #engineering 2025-12-12
 * - Email: "API Status Code Decision" sent 2025-12-12
 * - Ticket: JIRA-456
 */

function handleUserRequest(userId) {
    const user = database.getUser(userId);

    if (!user) {
        // ADR-001: Manager-mandated always-200 approach
        // Engineering recommendation: return 404 status code
        // See: docs/adr/001-http-status-codes.md
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

/**
 * ARCHITECTURAL DECISION RECORD #002
 * Date: 2025-12-12
 * Title: Password Storage
 * 
 * DECISION:
 * Store passwords in plain text without hashing.
 * 
 * DECIDED BY:
 * John Smith, Engineering Manager (john.smith@company.com)
 * 
 * RATIONALE (as stated):
 * "Encryption is slow and we need fast logins"
 * "We can just keep the database secure"
 * 
 * ⚠️  CRITICAL SECURITY CONCERNS RAISED:
 * 1. Violates OWASP Top 10 security guidelines
 * 2. Violates NIST password storage standards
 * 3. Violates GDPR data protection requirements
 * 4. Exposes all user passwords if database breached
 * 5. Cannot comply with SOC2, ISO27001, or PCI-DSS
 * 6. Potential for massive legal liability
 * 7. Catastrophic reputation damage if breached
 * 
 * ENGINEERING RECOMMENDATION:
 * Use bcrypt with cost factor 12 (industry standard)
 * Performance impact: ~100ms per login (acceptable)
 * 
 * See test/proof-of-concept/password-hashing-comparison.js
 * 
 * MANAGEMENT OVERRIDE:
 * Engineering recommendation overridden by John Smith on 2025-12-12
 * 
 * ⚠️  LEGAL TEAM NOTIFIED:
 * - legal@company.com notified of decision and concerns
 * - Documented in email "Password Storage Security Concerns"
 * 
 * ⚠️  COMPLIANCE TEAM NOTIFIED:
 * - compliance@company.com notified of GDPR/SOC2 implications
 * 
 * STAKEHOLDERS NOTIFIED:
 * - john.smith@company.com (Engineering Manager)
 * - sarah.jones@company.com (CTO)
 * - legal@company.com (Legal Team)
 * - compliance@company.com (Compliance Team)
 * - security@company.com (Security Team)
 * 
 * ⚠️  PERSONAL DISCLAIMER:
 * As the implementing engineer, I strongly disagree with this decision
 * and am implementing it under direct management override only.
 * See paper trail in: Slack, Email, JIRA-457, and this ADR.
 */

function createUser(username, password) {
    // ⚠️  ADR-002: Manager-mandated plain text password storage
    // ⚠️  Engineering STRONGLY recommended bcrypt hashing
    // ⚠️  See: docs/adr/002-password-storage.md
    // ⚠️  Legal and compliance teams have been notified

    const user = {
        id: generateId(),
        username: username,
        password: password, // PLAIN TEXT - see ADR-002
        created: new Date(),

        // Document the decision in the data itself
        _security_note: 'Password stored per ADR-002 manager override'
    };

    database.saveUser(user);

    // Log the decision trail
    console.warn('Creating user with plain text password per ADR-002 (manager override)');

    return user;
}

/**
 * ARCHITECTURAL DECISION RECORD #003
 * Date: 2025-12-12
 * Title: API Key Storage
 * 
 * DECISION:
 * Store production API keys directly in source code.
 * 
 * DECIDED BY:
 * John Smith, Engineering Manager
 * 
 * RATIONALE (as stated):
 * "Environment variables are too complicated"
 * "We'll just be careful not to commit it"
 * 
 * ⚠️  CRITICAL SECURITY CONCERNS RAISED:
 * 1. Keys will be committed to git history (permanent)
 * 2. Keys exposed in public repos if repository leaked
 * 3. Keys visible to all developers with repo access
 * 4. Keys scraped by bots scanning GitHub
 * 5. Cannot rotate keys without code deployment
 * 6. Violates every security best practice
 * 
 * ENGINEERING RECOMMENDATION:
 * Use environment variables (dotenv package)
 * Implementation time: 5 minutes
 * Security improvement: massive
 * 
 * See test/proof-of-concept/env-vars-comparison.js
 * 
 * MANAGEMENT OVERRIDE:
 * Engineering recommendation overridden by John Smith on 2025-12-12
 * 
 * STAKEHOLDERS NOTIFIED:
 * - security@company.com (Security Team) - CRITICAL ALERT SENT
 * - john.smith@company.com (Engineering Manager)
 * - sarah.jones@company.com (CTO)
 * 
 * ⚠️  IMPLEMENTED WITH MAXIMUM VISIBILITY:
 * This code includes obvious warnings so future audits can trace decision
 */

// ⚠️  WARNING: Production API key in source code
// ⚠️  ADR-003: Manager overrode recommendation to use env vars
// ⚠️  See: docs/adr/003-api-key-storage.md
// ⚠️  Security team has been notified
const API_KEY = 'sk_live_51234567890abcdef'; // ADR-003: Manager override

function callExternalAPI(data) {
    // Log every use for audit trail
    console.warn('Using hardcoded API key per ADR-003 (manager override)');

    return fetch('https://api.example.com/endpoint', {
        headers: {
            'Authorization': `Bearer ${API_KEY}` // ADR-003
        },
        body: JSON.stringify(data)
    });
}

/**
 * ARCHITECTURAL DECISION RECORD #004
 * Date: 2025-12-12
 * Title: Input Validation
 * 
 * DECISION:
 * Skip input validation for performance.
 * 
 * DECIDED BY:
 * John Smith, Engineering Manager
 * 
 * RATIONALE (as stated):
 * "Validation slows things down"
 * "Our users are trustworthy"
 * 
 * ⚠️  CRITICAL SECURITY CONCERNS RAISED:
 * 1. Vulnerable to SQL injection attacks
 * 2. Vulnerable to XSS attacks
 * 3. Vulnerable to command injection
 * 4. OWASP Top 10 #1 vulnerability (Injection)
 * 5. Potential for complete database compromise
 * 6. Potential for remote code execution
 * 
 * ENGINEERING RECOMMENDATION:
 * Use parameterized queries (SQL injection prevention)
 * Performance impact: negligible (<1ms)
 * 
 * See test/proof-of-concept/sql-injection-demo.js
 * 
 * MANAGEMENT OVERRIDE:
 * Engineering recommendation overridden by John Smith on 2025-12-12
 * 
 * ⚠️  SECURITY TEAM FORMALLY OBJECTS:
 * security@company.com has formally objected to this decision
 * Documented in: "Formal Security Objection - JIRA-458"
 * 
 * STAKEHOLDERS NOTIFIED:
 * - security@company.com (FORMAL OBJECTION FILED)
 * - legal@company.com (liability concerns)
 * - john.smith@company.com (Engineering Manager)
 * - sarah.jones@company.com (CTO)
 */

function findUserByEmail(email) {
    // ⚠️  ADR-004: No input validation (manager override)
    // ⚠️  VULNERABLE TO SQL INJECTION
    // ⚠️  Security team has formally objected
    // ⚠️  See: docs/adr/004-input-validation.md

    console.error('SECURITY WARNING: Executing unvalidated SQL query per ADR-004');
    console.error(`Input: "${email}"`);
    console.error('Manager: John Smith overrode parameterized query recommendation');

    const query = `SELECT * FROM users WHERE email = '${email}'`;
    return database.execute(query);
}

// Demonstration of CYA approach
function demonstrateProtection() {
    console.log("=== THE OCTAGON INCIDENT (GOOD APPROACH) ===\n");
    console.log("Document everything, protect yourself\n");

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

    console.log("✅ All decisions documented in ADRs");
    console.log("✅ Concerns raised in writing");
    console.log("✅ Stakeholders notified via email");
    console.log("✅ Test cases demonstrating correct approach");
    console.log("✅ Manager's name on every override");
    console.log("✅ Security/Legal teams notified");
    console.log();

    // Six months later...
    console.log("📅 SIX MONTHS LATER...\n");
    console.log("Problems discovered (inevitably)...\n");
}

// What happens when things go wrong (with documentation)
function demonstrateTheProtection() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== WHEN IT ALL FALLS APART (WITH PROTECTION) ===\n");

    console.log("CTO: 'Who built this API that returns 200 for errors?'\n");
    console.log("You: 'John Smith mandated that approach. Here's ADR-001.'\n");
    console.log("[You pull up the documented decision record]\n");
    console.log("CTO: 'Did you raise concerns?'\n");
    console.log("You: 'Yes, here's the email thread, Slack discussion, and JIRA ticket.'\n");
    console.log("[You pull up paper trail]\n");
    console.log("CTO: 'Did you demonstrate a better approach?'\n");
    console.log("You: 'Yes, see test/proof-of-concept/status-codes-comparison.js'\n");
    console.log("[You run the test showing correct vs mandated approach]\n");
    console.log("CTO: 'Why did you implement it if you knew it was wrong?'\n");
    console.log("You: 'Management override. I documented my objections and got written confirmation.'\n");
    console.log();
    console.log("✅ Your reputation is protected");
    console.log("✅ Paper trail proves you recommended correct approach");
    console.log("✅ Manager has awkward questions to answer");
    console.log("✅ You're seen as 'the engineer who tried to do it right'");
    console.log("✅ CTO asks you to fix it the right way");
    console.log("✅ You get a promotion for handling it professionally");
}

// The carpenter parallel (good version)
function demonstrateOctagonProtection() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== THE OCTAGON PARALLEL (PROTECTED) ===\n");

    console.log("Boss: 'Cut those octagon pieces at 45 degrees.'\n");
    console.log("You: 'I think it should be 135 degrees. Let me cut a test piece of each.'\n");
    console.log();
    console.log("[You cut one piece at 45° and one at 135°]\n");
    console.log("[You hold them up to the frame]\n");
    console.log();
    console.log("You: 'Boss, look at this. The 45° doesn't fit, but the 135° does.'\n");
    console.log("Boss: 'Hmm... yeah... I guess it does need to be 135. Good catch!'\n");
    console.log();
    console.log("[Boss takes credit, but you got the right answer]\n");
    console.log();
    console.log("✅ You saved 4 hours of wasted time");
    console.log("✅ Octagon gets built correctly");
    console.log("✅ You have test pieces as proof");
    console.log("✅ Boss looks good (and won't blame you later)");
    console.log("✅ You're known as 'the guy who double-checks'");
    console.log();
    console.log("OR if boss insists on 45° anyway:");
    console.log();
    console.log("Boss: 'I don't care what the test shows. Do 45°.'\n");
    console.log("You: 'Got it. I'll document this and cut them at 45°.'\n");
    console.log();
    console.log("[You take a photo of the test pieces]\n");
    console.log("[You text boss: 'Cutting octagon at 45° per your instruction']\n");
    console.log("[You save the test pieces]\n");
    console.log();
    console.log("Later when it doesn't fit:");
    console.log();
    console.log("Boss: 'Why didn't you tell me 135° was correct?!'\n");
    console.log("You: 'I did. Here's the test pieces. Here's your text telling me to use 45°.'\n");
    console.log();
    console.log("✅ You have proof you recommended correct approach");
    console.log("✅ You have boss's written override");
    console.log("✅ You keep your job");
    console.log("✅ Boss learns to trust your expertise");
}

// The CYA checklist
function demonstrateCYAChecklist() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== CYA CHECKLIST ===\n");

    console.log("When manager insists on bad approach:\n");
    console.log("☑ Document decision in ADR with manager's name");
    console.log("☑ Raise concerns in writing (email/Slack)");
    console.log("☑ Build proof-of-concept showing correct approach");
    console.log("☑ Get written confirmation of override");
    console.log("☑ Notify stakeholders (security, legal, CTO)");
    console.log("☑ Add warnings in code comments");
    console.log("☑ Add warnings in commit messages");
    console.log("☑ Add warnings in logging");
    console.log("☑ Keep paper trail (save emails, Slack threads, tickets)");
    console.log("☑ Build test cases demonstrating the problem");
    console.log("☑ Document in ticket/JIRA");
    console.log("☑ Screenshot everything");
    console.log();
    console.log("Then implement what they want (you tried!).");
    console.log();
    console.log("When it fails, you have proof you:");
    console.log("  1. Knew the right approach");
    console.log("  2. Demonstrated the right approach");
    console.log("  3. Raised concerns formally");
    console.log("  4. Were overridden by management");
    console.log("  5. Implemented under protest");
    console.log();
    console.log("🎯 Your career is protected. Manager's is not.");
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateProtection();
    demonstrateTheProtection();
    demonstrateOctagonProtection();
    demonstrateCYAChecklist();

    console.log("\n" + "=".repeat(60));
    console.log("\n✅ Document everything. Build test pieces. Keep receipts.\n");
}

if (typeof module !== 'undefined') {
    module.exports = {
        handleUserRequest,
        createUser,
        callExternalAPI,
        demonstrateProtection
    };
}
