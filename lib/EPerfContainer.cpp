#include "../include/EPerf/EPerf.h"
#include "../include/EPerf/EPerfHelpers.h"

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

/*
    q   << "INSERT OR IGNORE INTO kernels (id, name) VALUES("
        << id << ", '" << name << "')";
*/

}

}
