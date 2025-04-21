#include "../include/security_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include "../include/database.h"

void generate_salt(char *salt, int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        salt[i] = charset[key];
    }
    salt[length - 1] = '\0';
}

void hash_password(const char *password, const char *salt, char *hash) {
    char salted[256];
    snprintf(salted, sizeof(salted), "%s%s", password, salt);
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)salted, strlen(salted), digest);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&hash[i*2], "%02x", (unsigned int)digest[i]);
    hash[HASH_SIZE-1] = '\0';
}

bool verify_password(const char *password, const char *salt, const char *hash) {
    char calc_hash[HASH_SIZE];
    hash_password(password, salt, calc_hash);
    return strcmp(calc_hash, hash) == 0;
}

int check_login_attempts(const char *reg_number) {
    const char *sql = "SELECT attempts FROM login_attempts WHERE reg_number = ?;";
    sqlite3_stmt *stmt;
    int attempts = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            attempts = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return attempts;
}

void record_failed_login(const char *reg_number) {
    const char *sql = "INSERT INTO login_attempts (reg_number, attempts, last_attempt) VALUES (?, 1, strftime('%s','now')) "
                      "ON CONFLICT(reg_number) DO UPDATE SET attempts = attempts + 1, last_attempt = strftime('%s','now');";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    int attempts = check_login_attempts(reg_number);
    if (attempts >= MAX_LOGIN_ATTEMPTS) {
        const char *lock_sql = "UPDATE login_attempts SET locked_until = strftime('%s','now') + ? WHERE reg_number = ?;";
        if (sqlite3_prepare_v2(db, lock_sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, LOCKOUT_DURATION);
            sqlite3_bind_text(stmt, 2, reg_number, -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
}

void reset_login_attempts(const char *reg_number) {
    const char *sql = "DELETE FROM login_attempts WHERE reg_number = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

bool is_account_locked(const char *reg_number) {
    const char *sql = "SELECT locked_until FROM login_attempts WHERE reg_number = ?;";
    sqlite3_stmt *stmt;
    int locked_until = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, reg_number, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            locked_until = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    int now = (int)time(NULL);
    return locked_until > now;
}
