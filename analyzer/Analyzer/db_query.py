'''
Created on Dec 11, 2012

@author: bzcdress
'''

#import sqlite3
import apsw

DATABASE = 'static/db/eperf.db'

class db_query:
    def __init__(self):
        #self.db = sqlite3.connect(DATABASE)
        self.db = apsw.Connection(DATABASE)
        
    def db_query(self, query):
        cursor = self.db.cursor()
        return list(cursor.execute(query))
        
    def db_query_full_table(self, table):
        return self.db_query("SELECT * FROM " + table)
        
    def db_query_experiments(self):
        return self.db_query("SELECT DISTINCT id_experiment FROM data;")
        
    def db_query_experiment_overview(self, e_id):
        
        kernels = self.db_query(
            "SELECT DISTINCT kernels.id, kernels.name \
             FROM kernels \
             INNER JOIN data on kernels.id = data.id_kernel \
             WHERE id_experiment = '" + e_id + "'")

        devices = self.db_query(
            "SELECT DISTINCT devices.id, devices.name \
             FROM devices \
             INNER JOIN data on devices.id = data.id_device \
             WHERE id_experiment = '" + e_id + "'")
        
        return [kernels, devices]
        
    def db_query_kernels(self):
        return self.db_query_full_table("kernels");
    
    def db_query_devices(self):
        return self.db_query_full_table("devices");
    
    def db_query_data_table(self, d, k, E):
        
        # [u'0', u'00000000-0000-0000-604a-feded57f0000', u'1', u'00000000-0000-0000-604a-feded57f0000', u'2', u'00000000-0000-0000-604a-feded57f0000']
        
        ranges = dict();
        for e in E:
            ranges[e] = self.db_query(
                "SELECT min(ts_start_s + ts_start_ns * 1.0e-9) \
                 FROM data \
                 WHERE id_experiment = '" + e + "'")[0][0]
            
        devices = str()
        kernels = str()

        for i in range(0, len(d), 2):
            devices += " (id_device = " + d[i] + " AND id_experiment = '" + d[i+1] + "') OR"

        for i in range(0, len(k), 2):
            kernels += " (id_kernel = " + k[i] + " AND id_experiment = '" + k[i+1] + "') OR"

        devices = devices[:len(devices) - 3]
        kernels = kernels[:len(kernels) - 3]
        
        query = "SELECT \
                    tid, \
                    ts_start_s + ts_start_ns * 1.0e-9, \
                    ts_stop_s + ts_stop_ns * 1.0e-9, \
                    id_experiment, id_kernel, id_device \
                 FROM data WHERE "
                     
        if (len(devices) != 0 and len(kernels) != 0):
            query += kernels + " AND " + devices
        else:
            query += kernels + devices
                    
        query += " ORDER BY tid ASC, ts_start_s ASC, ts_start_ns ASC"
        
        result = self.db_query(query)
        
        result_list = [];
        for r in result:
            r = list(r)
            r[1] -= ranges[r[3]]
            r[2] -= ranges[r[3]]
            
            result_list.append(r);
       
        return result_list;
    