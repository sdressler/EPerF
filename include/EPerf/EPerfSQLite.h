#ifndef EPERF_SQLITE_H
#define EPERF_SQLITE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <inttypes.h>

namespace ENHANCE {
class IEPerfSQLite {
public:
    virtual std::vector<std::string> createSQLInsertObj() const = 0;
    virtual ~IEPerfSQLite() { }
};

class EPerfSQLite {

private:
    std::string db_file;
    sqlite3 *db;
    
    void initializeDBIfNeeded() {
        
        sqlite3_stmt *stmt;

        std::string sql = "SELECT COUNT(*) FROM devices";
        int ret = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

        if (ret != SQLITE_OK) {

            // DB seems to be empty, so initialize it
            
            sql = "CREATE TABLE devices ( \
                    id INTEGER PRIMARY KEY, \
                    name VARCHAR(255) \
                   )";

            executeInsertQuery(sql);

            sql = "CREATE TABLE subdevices ( \
                    id_left INTEGER, \
                    id_right INTEGER, \
                    PRIMARY KEY(id_left, id_right) \
                   )";

            executeInsertQuery(sql);

            sql = "CREATE TABLE kernels ( \
                    id INTEGER PRIMARY KEY, \
                    name VARCHAR(255) \
                   )";

            executeInsertQuery(sql);
            sql = "CREATE TABLE experiments ( \
                    id INTEGER PRIMARY KEY, \
                    date INTEGER, \
                    name VARCHAR(255), \
                    start_s INTEGER, \
                    start_ns INTEGER \
                   )";

            executeInsertQuery(sql);

/*
            sql = "CREATE TABLE kernelHasConfigurations ( \
                    id_kernel INTEGER, \
                    hash VARCHAR(64), \
                    PRIMARY KEY(id_kernel, hash) \
                   )";

            executeInsertQuery(sql);
*/
/*
            sql = "CREATE TABLE kernelConfigurations ( \
                    hash VARCHAR(64), \
                    key VARCHAR(255), \
                    value VARCHAR(255), \
                    PRIMARY KEY(hash, key, value) \
                   )";

            executeInsertQuery(sql);
*/
            sql = "CREATE TABLE data ( \
                    id_kernel INTEGER, \
                    id_device INTEGER, \
                    ts_start INTEGER, \
                    ts_stop INTEGER, \
                    cpuclock_start INTEGER, \
                    cpuclock_stop INTEGER, \
                    tid INTEGER, \
                    data_in INTEGER, \
                    data_out INTEGER, \
                    id_experiment INTEGER, \
                    PRIMARY KEY( \
                        id_kernel, id_device, tid, ts_start \
                    ) \
                   )";

            executeInsertQuery(sql);
        
        }

    }

protected:

public:
    EPerfSQLite(std::string _db_file = "") : db_file(_db_file) {

        if (_db_file == "") {
            db_file = std::string("eperf.db");
        }

        int ret = sqlite3_open(db_file.c_str(), &db);

        if (ret != SQLITE_OK) {
            std::stringstream err;
            err << "Could not open SQLite DB. Reason: " << sqlite3_errmsg(db);
            err << " Path was: " << db_file;
            throw std::runtime_error(err.str());
        }

        initializeDBIfNeeded();
    
    }

    virtual ~EPerfSQLite() { sqlite3_close(db); }

    uint64_t getLastExperimentID() const;

    void executeInsertQuery(const std::string &q) {

//        std::cout << q << "\n";

        sqlite3_stmt *stmt;

        int ret = sqlite3_prepare_v2(db, q.c_str(), q.size(), &stmt, NULL);

        if (ret != SQLITE_OK) {
            std::stringstream err;
            err << "Could not SELECT from SQLite DB. Reason: " << sqlite3_errmsg(db);
            throw std::runtime_error(err.str());
        }
        
        ret = sqlite3_step(stmt);

        if (ret != SQLITE_DONE) {
            std::stringstream err;
            err << "Could not execute: " << q << "\nError: " << sqlite3_errmsg(db);
            throw std::runtime_error(err.str());
        }

        sqlite3_finalize(stmt);

    }

    void executeInsertQuery(const std::vector<std::string> &vq) {

        std::vector<std::string>::const_iterator it;
        for (it = vq.begin(); it != vq.end(); ++it) {
            executeInsertQuery(*it);
        }

    }

    void beginTransaction() { sqlite3_exec(db, "BEGIN", 0, 0, 0); }
    void endTransaction() { sqlite3_exec(db, "COMMIT", 0, 0, 0); }

};
}

#endif /* EPERF_SQLITE_H */
