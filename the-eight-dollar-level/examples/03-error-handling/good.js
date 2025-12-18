// Explicit Error Handling - Information Mode
// The $800 carpenter who calmly investigates why the rafter doesn't fit

// Result type - makes success/failure explicit
const Result = {
    success: (value) => ({ success: true, value }),
    failure: (error, context = {}) => ({ success: false, error, context })
};

// Explicit error codes - no surprises
const ErrorCodes = {
    // Validation errors
    FILE_REQUIRED: 'FILE_REQUIRED',
    FILE_NAME_REQUIRED: 'FILE_NAME_REQUIRED',
    FILE_TOO_LARGE: 'FILE_TOO_LARGE',
    INVALID_FILE_TYPE: 'INVALID_FILE_TYPE',

    // Storage errors
    INVALID_USER_ID: 'INVALID_USER_ID',
    STORAGE_QUOTA_EXCEEDED: 'STORAGE_QUOTA_EXCEEDED',
    DATABASE_ERROR: 'DATABASE_ERROR',

    // Security errors
    VIRUS_DETECTED: 'VIRUS_DETECTED',
    VIRUS_SCAN_UNAVAILABLE: 'VIRUS_SCAN_UNAVAILABLE',

    // Storage errors
    DISK_WRITE_FAILED: 'DISK_WRITE_FAILED',
    NETWORK_TIMEOUT: 'NETWORK_TIMEOUT'
};

// Validation with explicit error returns
function validateFile(file) {
    if (!file) {
        return Result.failure(ErrorCodes.FILE_REQUIRED);
    }

    if (!file.name) {
        return Result.failure(ErrorCodes.FILE_NAME_REQUIRED);
    }

    if (!file.size || file.size > 10 * 1024 * 1024) {
        return Result.failure(ErrorCodes.FILE_TOO_LARGE, {
            maxSize: 10 * 1024 * 1024,
            actualSize: file.size
        });
    }

    const allowedTypes = ['image/jpeg', 'image/png', 'image/gif'];
    if (!allowedTypes.includes(file.type)) {
        return Result.failure(ErrorCodes.INVALID_FILE_TYPE, {
            allowedTypes,
            actualType: file.type
        });
    }

    return Result.success(file); // Explicit success
}

// Storage check with explicit error returns
function checkStorage(userId) {
    const storageResult = getUserStorage(userId);
    if (!storageResult.success) {
        return storageResult; // Pass error up the chain
    }

    const storage = storageResult.value;

    if (storage.used + storage.pending > storage.quota) {
        return Result.failure(ErrorCodes.STORAGE_QUOTA_EXCEEDED, {
            used: storage.used,
            pending: storage.pending,
            quota: storage.quota,
            available: storage.quota - storage.used - storage.pending
        });
    }

    return Result.success(storage);
}

function getUserStorage(userId) {
    if (!userId || userId < 0) {
        return Result.failure(ErrorCodes.INVALID_USER_ID, { userId });
    }

    if (userId === 999) {
        return Result.failure(ErrorCodes.DATABASE_ERROR, {
            message: "Connection failed"
        });
    }

    return Result.success({
        used: 50 * 1024 * 1024,
        pending: 5 * 1024 * 1024,
        quota: 100 * 1024 * 1024
    });
}

// Virus scan with explicit error returns
function scanForVirus(file) {
    if (Math.random() < 0.1) { // 10% chance
        return Result.failure(ErrorCodes.VIRUS_SCAN_UNAVAILABLE, {
            canRetry: true,
            retryAfter: 5000
        });
    }

    if (file.name.includes('virus')) {
        return Result.failure(ErrorCodes.VIRUS_DETECTED, {
            fileName: file.name,
            quarantined: true
        });
    }

    return Result.success({ clean: true });
}

// File save with explicit error returns
function saveFile(file, userId) {
    if (Math.random() < 0.05) { // 5% chance
        return Result.failure(ErrorCodes.DISK_WRITE_FAILED, {
            canRetry: true,
            availableFallbacks: ['s3', 'backup-server']
        });
    }

    if (Math.random() < 0.05) { // 5% chance
        return Result.failure(ErrorCodes.NETWORK_TIMEOUT, {
            canRetry: true,
            timeout: 30000
        });
    }

    return Result.success({
        fileId: Math.random().toString(36).substr(2, 9),
        url: `/uploads/${userId}/${file.name}`
    });
}

// The main function - explicit about what can fail
function uploadFile(userId, file) {
    // Each step returns Result<T> - success or failure

    const validationResult = validateFile(file);
    if (!validationResult.success) {
        return validationResult; // Return explicit error
    }

    const storageResult = checkStorage(userId);
    if (!storageResult.success) {
        return storageResult; // Return explicit error
    }

    const scanResult = scanForVirus(file);
    if (!scanResult.success) {
        return scanResult; // Return explicit error
    }

    const saveResult = saveFile(file, userId);
    if (!saveResult.success) {
        return saveResult; // Return explicit error
    }

    return saveResult; // Return explicit success
}

// Demonstration of explicit error handling
function demonstrateExplicitErrors() {
    console.log("=== EXPLICIT ERROR HANDLING ===\n");
    console.log("Clear failure modes - every function's contract is visible\n");

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

        const result = uploadFile(test.userId, test.file);

        if (result.success) {
            console.log(`   ✅ Success: ${result.value.url}`);
            successCount++;
        } else {
            console.log(`   ❌ Failed: ${result.error}`);
            if (Object.keys(result.context).length > 0) {
                console.log(`   📋 Context:`, result.context);
            }

            // Now we can handle each error specifically!
            handleError(result.error, result.context);
            failureCount++;
        }

        console.log();
    });

    console.log("=== BENEFITS OF THIS APPROACH ===\n");
    console.log("1. Function signature is honest - returns Result<T>");
    console.log("2. All possible errors are enumerated in ErrorCodes");
    console.log("3. Error context provides actionable information");
    console.log("4. Can't ignore errors - must check success field");
    console.log("5. Errors are just data - easy to test and handle");
    console.log("6. Clear separation between success and failure paths");
    console.log();
    console.log(`Results: ${successCount} succeeded, ${failureCount} failed`);
    console.log();
    console.log("✅ Like a carpenter who calmly checks why the rafter doesn't fit");
}

// Specific error handling based on error codes
function handleError(errorCode, context) {
    switch (errorCode) {
        case ErrorCodes.FILE_REQUIRED:
            console.log("   🔧 Fix: User needs to select a file");
            break;

        case ErrorCodes.FILE_TOO_LARGE:
            console.log(`   🔧 Fix: Compress file or select one under ${context.maxSize / 1024 / 1024}MB`);
            break;

        case ErrorCodes.INVALID_FILE_TYPE:
            console.log(`   🔧 Fix: Use one of: ${context.allowedTypes.join(', ')}`);
            break;

        case ErrorCodes.STORAGE_QUOTA_EXCEEDED:
            console.log(`   🔧 Fix: Delete old files or upgrade storage (${context.available} bytes available)`);
            break;

        case ErrorCodes.VIRUS_DETECTED:
            console.log(`   🔧 Fix: File quarantined, contact security team`);
            break;

        case ErrorCodes.VIRUS_SCAN_UNAVAILABLE:
            console.log(`   🔧 Fix: Retry in ${context.retryAfter}ms`);
            break;

        case ErrorCodes.DISK_WRITE_FAILED:
            console.log(`   🔧 Fix: Retry or use fallback: ${context.availableFallbacks.join(', ')}`);
            break;

        case ErrorCodes.NETWORK_TIMEOUT:
            console.log(`   🔧 Fix: Retry with ${context.timeout}ms timeout`);
            break;

        default:
            console.log(`   🔧 Fix: Unknown error - check logs`);
    }
}

// Clean sequential handling - no nested try-catch
function demonstrateCleanHandling() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== CLEAN SEQUENTIAL HANDLING ===\n");

    const userId = 123;
    const file = { name: 'photo.jpg', size: 1024, type: 'image/jpeg' };

    console.log("Processing upload with explicit error checking...\n");

    const validationResult = validateFile(file);
    if (!validationResult.success) {
        console.log("❌ Validation failed:", validationResult.error);
        handleError(validationResult.error, validationResult.context);
        return;
    }
    console.log("✅ Validation passed");

    const storageResult = checkStorage(userId);
    if (!storageResult.success) {
        console.log("❌ Storage check failed:", storageResult.error);
        handleError(storageResult.error, storageResult.context);
        return;
    }
    console.log("✅ Storage check passed");

    const scanResult = scanForVirus(file);
    if (!scanResult.success) {
        console.log("❌ Virus scan failed:", scanResult.error);
        handleError(scanResult.error, scanResult.context);
        return;
    }
    console.log("✅ Virus scan passed");

    const saveResult = saveFile(file, userId);
    if (!saveResult.success) {
        console.log("❌ Save failed:", saveResult.error);
        handleError(saveResult.error, saveResult.context);
        return;
    }
    console.log("✅ File saved:", saveResult.value.url);

    console.log("\n📊 Clear flow - no surprises, no nested try-catch");
}

// Production quality - specific error messages
function demonstrateProductionHandling() {
    console.log("\n" + "=".repeat(60) + "\n");
    console.log("=== PRODUCTION-QUALITY ERROR HANDLING ===\n");

    const uploads = [
        { userId: 123, file: { name: 'valid.jpg', size: 1024, type: 'image/jpeg' } },
        { userId: -1, file: { name: 'test.jpg', size: 1024, type: 'image/jpeg' } },
        { userId: 123, file: null },
    ];

    uploads.forEach((upload, i) => {
        const result = uploadFile(upload.userId, upload.file);

        if (result.success) {
            console.log(`Upload ${i + 1}: ✅ Success`);
            console.log(`   User sees: "File uploaded successfully!"`);
        } else {
            console.log(`Upload ${i + 1}: ❌ Failed (${result.error})`);

            // Show user-friendly message based on specific error
            const userMessage = getUserMessage(result.error, result.context);
            console.log(`   User sees: "${userMessage}"`);
            console.log(`   📋 Context available for debugging:`, result.context);
        }
        console.log();
    });

    console.log("This is the $800 level:");
    console.log("  - Rafter doesn't fit");
    console.log("  - Calmly check measurements");
    console.log("  - Discover beam is warped 1/4 inch");
    console.log("  - Cut new rafter accounting for warp");
    console.log("  - Perfect fit, 10 minutes total");
}

function getUserMessage(errorCode, context) {
    switch (errorCode) {
        case ErrorCodes.FILE_REQUIRED:
            return "Please select a file to upload.";
        case ErrorCodes.FILE_TOO_LARGE:
            return `File is too large. Maximum size is ${context.maxSize / 1024 / 1024}MB.`;
        case ErrorCodes.INVALID_FILE_TYPE:
            return `Invalid file type. Accepted types: ${context.allowedTypes.join(', ')}`;
        case ErrorCodes.STORAGE_QUOTA_EXCEEDED:
            return "Storage quota exceeded. Please delete some files or upgrade your plan.";
        case ErrorCodes.INVALID_USER_ID:
            return "Invalid user account. Please log in again.";
        case ErrorCodes.VIRUS_DETECTED:
            return "Security warning: File may contain malicious content.";
        default:
            return "Upload failed. Please try again.";
    }
}

// Run demonstrations
if (typeof module === 'undefined' || require.main === module) {
    demonstrateExplicitErrors();
    demonstrateCleanHandling();
    demonstrateProductionHandling();

    console.log("\n" + "=".repeat(60));
    console.log("\n🎯 Errors as data = Calm investigation, not panic\n");
}

if (typeof module !== 'undefined') {
    module.exports = {
        Result,
        ErrorCodes,
        uploadFile,
        validateFile,
        checkStorage,
        demonstrateExplicitErrors
    };
}
