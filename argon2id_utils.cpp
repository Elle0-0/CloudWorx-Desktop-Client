#include "argon2id_utils.h"
#include <sodium.h>
#include <QDebug>
#include <QRegularExpression>

QString hashPassword(const QByteArray& password) {
    if (sodium_init() < 0) {
        qDebug() << "libsodium init failed!";
        return {};
    }

    char hashed[crypto_pwhash_STRBYTES];

    // Explicit parameters to match Web app
    if (crypto_pwhash_str_alg(
            hashed,
            password.constData(),
            static_cast<unsigned long long>(password.length()),
            3,                      // timeCost
            12288,               // memoryCost = 12 MiB
            crypto_pwhash_ALG_ARGON2ID13
            ) != 0) {
        qDebug() << "Out of memory!";
        return {};
    }

    return QString(hashed);
}

bool verifyPassword(const QByteArray& password, const QString& hash) {

    bool result = crypto_pwhash_str_verify(
                      hash.toUtf8().constData(),
                      password.constData(),
                      static_cast<unsigned long long>(password.length())
                      ) == 0;
    return result;
}

bool isPasswordNISTCompliant(const QString& password, QString& errorMessage) {
    QStringList issues;

    // Static regex patterns for efficiency
    static const QRegularExpression uppercaseRegex("[A-Z]");
    static const QRegularExpression lowercaseRegex("[a-z]");
    static const QRegularExpression numberRegex("[0-9]");
    static const QRegularExpression specialCharRegex("[^a-zA-Z0-9]");

    // our custom requirements
    if (password.length() < 12) {
        issues << "Password must be at least 12 characters long.";
    }

    if (!uppercaseRegex.match(password).hasMatch()) {
        issues << "Password must contain at least one uppercase letter.";
    }

    if (!lowercaseRegex.match(password).hasMatch()) {
        issues << "Password must contain at least one lowercase letter.";
    }

    if (!numberRegex.match(password).hasMatch()) {
        issues << "Password must contain at least one number.";
    }

    if (!specialCharRegex.match(password).hasMatch()) {
        issues << "Password must contain at least one special character.";
    }

    // NIST requirement: Maximum length
    if (password.length() > 64) {
        issues << "Password must be no more than 64 characters long.";
    }


    // Enhanced common password check
    QString lower = password.toLower();
    static const QStringList commonPasswords = {
        // Common patterns that meet your composition requirements
        "password123!", "password1!", "welcome123!", "admin123!",
        "qwerty123!", "letmein1!", "password12!", "welcome12!",
        "admin1234!", "qwerty12!", "hello123!", "login123!",
        "master123!", "dragon123!", "monkey123!", "abc123!",
        "test123!", "user123!", "pass123!", "temp123!",
        "default123!", "system123!", "root123!", "guest123!",
        // Keyboard patterns with required chars
        "qwerty123!", "asdf123!", "zxcv123!", "1234qwer!",
        // Simple substitutions
        "p@ssw0rd123", "passw0rd1!", "p@ssword123", "welc0me123!",
        // Common corporate patterns
        "company123!", "winter2024!", "spring2024!", "summer2024!",
        "january2024!", "february2024!"
    };

    for (const QString& common : commonPasswords) {
        if (lower == common) {
            issues << "Password is too common. Please choose a more secure one.";
            break;
        }
    }

    // Check for repetitive pattern
    if (hasRepetitivePattern(password)) {
        issues << "Password contains repetitive patterns (like 'aaa' or '123').";
    }

    if (!issues.isEmpty()) {
        errorMessage = issues.join("\n");
        return false;
    }
    return true;
}

// Helper function for pattern detection
bool hasRepetitivePattern(const QString& password) {
    // Check for 3+ repeated characters (aaa, 111, etc.)
    for (int i = 0; i < password.length() - 2; i++) {
        if (password[i] == password[i+1] && password[i] == password[i+2]) {
            return true;
        }
    }

    // Check for simple sequences (123, abc, etc.)
    for (int i = 0; i < password.length() - 2; i++) {
        QChar c1 = password[i], c2 = password[i+1], c3 = password[i+2];
        if ((c1.unicode() + 1 == c2.unicode()) && (c2.unicode() + 1 == c3.unicode())) {
            return true;
        }
    }

    return false;
}

