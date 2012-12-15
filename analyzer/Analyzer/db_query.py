'''
Created on Dec 11, 2012

@author: bzcdress
'''

import apsw

class db_query:
    def __init__(self, db):
        self.db = apsw.Connection(db)
        
    def db_query(self, query):
        cursor = self.db.cursor()
        return list(cursor.execute(query))
        
    def db_query_full_table(self, table):
        return self.db_query("SELECT * FROM " + table)
        
    def db_query_experiments(self):
        return self.db_query("SELECT * FROM experiments;")
        
    def db_query_experiment_overview(self, e_id):
        return self.db_query(" \
            SELECT DISTINCT id_kernel, kernels.name, id_device, devices.name \
            FROM data \
            JOIN kernels ON id_kernel = kernels.id \
            JOIN devices ON id_device = devices.id \
            WHERE id_experiment = '" + e_id + "'")
        
    def db_query_kernels(self):
        return self.db_query_full_table("kernels");
    
    def db_query_devices(self):
        return self.db_query_full_table("devices");
    
    def db_query_data_table(self, selection):
        
        conditions = []
        ranges = dict()
        
        for s in selection:
            ranges[s[0]] = self.db_query(
                "SELECT min(ts_start_s + ts_start_ns * 1.0e-9) \
                 FROM data \
                 WHERE id_experiment = '" + s[0] + "'")[0][0]
                 
            conditions.append(
                     "id_experiment = '" + s[0] + "'" +
                " AND id_kernel = " + s[1] +
                " AND id_device = " + s[2]
            )

        query = "SELECT \
                    tid, \
                    ts_start_s + ts_start_ns * 1.0e-9, \
                    ts_stop_s + ts_stop_ns * 1.0e-9, \
                    id_experiment, id_kernel, id_device \
                 FROM data WHERE "
                 
        for c in conditions:
            query += "(" + c + ") OR "
              
        # Remove last "OR "       
        query = query[:len(query) - 3]
        
        # Append ordering             
        query += " ORDER BY tid ASC, ts_start_s ASC, ts_start_ns ASC"
        
        result = self.db_query(query)
        
        result_list = [];
        for r in result:
            r = list(r)
            r[1] -= ranges[r[3]]
            r[2] -= ranges[r[3]]
            
            result_list.append(r);
       
        return result_list;
    