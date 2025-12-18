// Exception-Based Error Handling - Panic Mode
// The $8 carpenter who throws the rafter when it doesn't fit

class FileUploadError extends Error {
    constructor(message) {
        super(message);
        this.name = 'FileUploadError';
    }
}

class ValidationError extends Error {
    constructor(message) {
        super(message);
        this.name = 'ValidationError';
    }
}

class StorageError extends Error {
    constructor(message) {
        super(message);
        this.name = 'StorageError';
    }
}

// Simulated file validation - exceptions hidden in implementation
function validateFile(file) {
    if (!file) {
        throw new ValidationError("File is required");
    }

    if (!file.name) {
        throw new ValidationError("File name is required");
    }

    // Hidden assumption - will crash if file.size doesn't exist
    if (file.size > 10 * 1024 * 1024) {
        throw new ValidationError("File too large (max 10MB)");
    }

    const allowedTypes = ['image/jpeg', 'image/png', 'image/gif'];
    if (!allowedTypes.includes(file.type)) {
        throw new ValidationError(`Invalid file type: ${file.type}`);
    }

    return true; // Or does it throw? Who knows!
}

// Simulated storage check - more hidden exceptions
function checkStorage(userId) {
    // What if userId is null? Exception!
    const storage = getUserStorage(userId);

    if (storage.used + storage.pending > storage.quota) {
        throw new StorageError("Storage quota exceeded");
    }

    return storage;
}

function getUserStorage(userId) {
    // Simulate database lookup
    if (!userId || userId < 0) {
        throw new Error("Invalid user ID"); // Different error type!
    }

    if (userId === 999) {
        throw new Error("Database connection failed"); // Yet another type!
    }

    return {
        used: 50 * 1024 * 1024,
        pending: 5 * 1024 * 1024,
        quota: 100 * 1024 * 1024
    };
}

// Simulated virus scan - guess what? More exceptions!
function scanForVirus(file) {
    if (Math.random() < 0.1) { // 10% chance
        throw new Error("Virus scan service unavailable");
    }

    if (file.name.includes('virus')) {
        throw new FileUploadError("Virus detected!");
    }

    return true;
}

// Simulated file save - the exception factory
function saveFile(file, userId) {
    if (Math.random() < 0.05) { // 5% chance
        throw new StorageError("Disk write failed");
    }

    if (Math.random() < 0.05) { // 5% chance  
        throw new Error("Network timeout");
    }

    return {
        fileId: Math.random().toString(36).substr(2, 9),
        url: `/uploads/${userId}/${file.name}`
    };
}

// The main function - a minefield of hidden exceptions
function uploadFile(userId, file) {
    // No indication from the signature that ANY of this can fail!

    validateFile(file); // Might throw ValidationError

    const storage = checkStorage(userId); // Might throw StorageError or Error

    scanForVirus(file); // Might throw FileUploadError or Error

    const result = saveFile(file, userId); // Might throw StorageError or Error

    return result; // Or maybe we threw somewhere and never got here
}

// Demonstration of exception chaos
function demonstrateExceptionChaos() {
    console.log("=== EXCEPTION-BASED ERROR HANDLING ===\n");
    console.log("Hidden failure modes - every function is a land mine\n");

    const testCases = [
        {
            name: "Valid file",
            userId: 123,
            file: { name: 'photo.jpg', size: 1024 * 1024, type: 'image/jpeg' }
        },
        {
            name: "Null file",
            userId: 123,
            file: null
        },
        {
            name: "File too large",
            userId: 123,
            file: { name: 'huge.jpg', size: 20 * 1024 * 1024, type: 'image/jpeg' }
        },
        {
            name: "Invalid type",
            userId: 123,
            file: { name: 'doc.pdf', size: 1024, type: 'application/pdf' }
        },
        {
            name: "Invalid user ID",
            userId: -1,
            file: { name: 'photo.jpg', size: 1024, type: 'image/jpeg' }
        },
        {
            name: "Virus in filename",
            userId: 123,
            file: { name: 'virus.jpg', size: 1024, type: 'image/jpeg' }
        }
    ];

    let successCount = 0;
    let failureCount = 0;

    testCases.forEach((test, i) => {
        console.log(`Test ${i + 1}: ${test.name}`);

        try {
            const result = uploadFile(test.userId, test.file);
            console.log(`   ✅ Success: ${result.url}`);
            successCount++;
        } catch (error) {
            console.log(`   ❌ Caught: ${error.name || 'Error'}: ${error.message}`);
            failureCount++;

            // Now what? We have no context about where it failed or how to recover
            // All we can do is show a generic error message
        }

        console.log();
    });

    console.log("=== PROBLEMS WITH THIS APPROACH ===\n");
    console.log("1. Function signature lies - doesn't show what can fail");
    console.log("2. Multiple exception types - hard to catch them all");
    console.log("3. Exceptions from deep in call stack - where did it fail?");
    console.log("4. No way to know all possible failures without reading all code");
    console.log("5. Catch-all handlers lose error context");
    console.log("6. Can't tell success from failure by return type");
    console.log();
    console.log(`Results: ${successCount} succeeded, ${failureCount} failed`);
    console.log();
    console.log("💥 Like a carpenter throwing rafters when they don't fit");
}

// Even worse - nested try-catch hell
function demonstrateNestedTryCatch() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== NESTED TRY-CATCH HELL ===\n");

    const userId = 123;
    const file = { name: 'photo.jpg', size: 1024, type: 'image/jpeg' };

    try {
        console.log("Attempting upload with defensive try-catch...\n");

        try {
            validateFile(file);
            console.log("✅ Validation passed");
        } catch (e) {
            console.log("❌ Validation failed:", e.message);
            throw e; // Re-throw because we can't handle it here
        }

        let storage;
        try {
            storage = checkStorage(userId);
            console.log("✅ Storage check passed");
        } catch (e) {
            console.log("❌ Storage check failed:", e.message);
            throw e;
        }

        try {
            scanForVirus(file);
            console.log("✅ Virus scan passed");
        } catch (e) {
            console.log("❌ Virus scan failed:", e.message);
            throw e;
        }

        let result;
        try {
            result = saveFile(file, userId);
            console.log("✅ File saved:", result.url);
        } catch (e) {
            console.log("❌ Save failed:", e.message);
            throw e;
        }

        return result;

    } catch (e) {
        console.log("\n🚨 Upload failed somewhere in the chain");
        console.log("🤷 Good luck figuring out how to recover!");
        throw e; // Pass the buck to the caller
    }
}

// Production nightmare - catch-all handler
function demonstrateCatchAll() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== CATCH-ALL HANDLER (PRODUCTION NIGHTMARE) ===\n");

    const uploads = [
        { userId: 123, file: { name: 'valid.jpg', size: 1024, type: 'image/jpeg' } },
        { userId: -1, file: { name: 'test.jpg', size: 1024, type: 'image/jpeg' } },
        { userId: 123, file: null },
    ];

    uploads.forEach((upload, i) => {
        try {
            const result = uploadFile(upload.userId, upload.file);
            console.log(`Upload ${i + 1}: ✅ Success`);
        } catch (e) {
            // This is what most production code does - terrible!
            console.log(`Upload ${i + 1}: ❌ Failed`);
            console.log("   Showing user: 'Something went wrong. Please try again.'");
            console.log(`   Actual error: ${e.message}`);
            console.log("   💀 User has no idea what went wrong or how to fix it");
        }
        console.log();
    });

    console.log("This is the $8 level:");
    console.log("  - Rafter doesn't fit");
    console.log("  - Throw it on the ground");
    console.log("  - Tell the boss 'it didn't work'");
    console.log("  - Boss: 'well... try again?'");
    console.log("  - 2 hours wasted");
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateExceptionChaos();
    demonstrateNestedTryCatch();
    demonstrateCatchAll();

    console.log("\n" + "=".repeat(60));
    console.log("\n💡 See good.js for the $800 level approach\n");
}

if (typeof module !== 'undefined') {
    module.exports = {
        uploadFile,
        validateFile,
        checkStorage,
        demonstrateExceptionChaos
    };
}
