#include "../include/EPerf/EPerf.h"

namespace ENHANCE {

void EPerf::commitToDB() {

    DMSG("Committing to DB");

    EPerfSQLite db(dbFileName);

    for (tDeviceMap::iterator it = devices.begin(); it != devices.end(); ++it) {

            db.executeInsertQuery(
                (it->second).createSQLInsertObj()
            );

    }

    for (tKernelMap::iterator it = kernels.begin(); it != kernels.end(); ++it) {

            db.executeInsertQuery(
                (it->second).createSQLInsertObj()
            );
    }

    db.beginTransaction();

    for (unsigned int i = 0; i < data.size(); i++) {

        //tDataVector::const_iterator it;
        std::list<EPerfData>::const_iterator it;
        for (it = data[i].begin(); it != data[i].end(); ++it) {
            db.executeInsertQuery(it->createSQLInsertObj());
        }

    }

    db.endTransaction();

    // Write the experiment
    std::stringstream q;

    q << "INSERT OR IGNORE INTO experiments (id, date, name) VALUES("
      << "'" << experiment_id   << "', "
             << experiment_date << ", "
      << "'" << experiment_name << "')";

    db.executeInsertQuery(q.str());

/*
    q   << "INSERT OR IGNORE INTO kernels (id, name) VALUES("
        << id << ", '" << name << "')";
*/

}

}
