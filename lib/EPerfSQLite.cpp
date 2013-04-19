/*
 * EPerfSQLite.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: bzcdress
 */

#include "../include/EPerf/EPerfSQLite.h"
#include "../include/EPerf/EPerfHelpers.h"

#include <limits>

namespace ENHANCE {
uint64_t EPerfSQLite::getLastExperimentID() const {

    sqlite3_stmt *stmt;
    std::string qry = "SELECT MAX(id) FROM experiments";

    int ret = sqlite3_prepare_v2(db, qry.c_str(), qry.size(), &stmt, NULL);

    if (ret != SQLITE_OK) {
        std::stringstream err;
        err << "Could not SELECT from SQLite DB. Reason: " << sqlite3_errmsg(db);
        throw std::runtime_error(err.str());
    }

    ret = sqlite3_step(stmt);

    uint64_t exp_id = std::numeric_limits<uint64_t>::max();
    /* FETCH DATA */
    while (ret == SQLITE_ROW) {
        //std::cout << sqlite3_column_int64(stmt, 0) << "\n";
        exp_id = sqlite3_column_int64(stmt, 0);
        DMSG("MAX EXP ID: " << exp_id)
        ret = sqlite3_step(stmt);
    }

    if (ret != SQLITE_DONE) {
        std::stringstream err;
        err << "Could not execute: " << qry << "\nError: " << sqlite3_errmsg(db);
        throw std::runtime_error(err.str());
    }

    if (exp_id == std::numeric_limits<uint64_t>::max()) {
        throw std::runtime_error("SQL statement successful, but exp_id was not set.");
    }

    sqlite3_finalize(stmt);

    return exp_id;
}
}
