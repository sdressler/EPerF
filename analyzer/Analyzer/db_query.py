'''
Created on Dec 11, 2012

@author: bzcdress
'''

import apsw
from collections import defaultdict

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
        
        #conditions = []
        #ranges = dict()
        
        order_query = " ORDER BY tid ASC, ts_start"
        
        result_dict = dict()
        
        for s in selection:
            
            # Get the overall minimum for the selected experiment
            s_min = self.db_query(
                "SELECT min(ts_start) " +
                "FROM data " +
                "WHERE id_experiment = '" + s[0] + "'"
            )[0][0]
           
            s_min_str = "%.9f" % (s_min)
           
            # Generate the base query 
            base_query = (
                "SELECT " +
                    "tid, " +
                    "ts_start - " + s_min_str + ", " +
                    "ts_stop - " + s_min_str +
                " FROM data WHERE "
            )
            
            # Get the data
            s_data = self.db_query(
                base_query + 
                     "id_experiment = '" + s[0] + "'" +
                " AND id_kernel = " + s[1] +
                " AND id_device = " + s[2] +
                order_query
            )
            
            temp_dict = defaultdict(list)
            
            for k, start, stop in s_data:
                key = str(s[0]) + "-" + str(s[1]) + "-" + str(s[2]) + "-" + str(k) # Key is a combination of experiment, kernel, device, thread
                temp_dict[key].append((start, stop)) 
            
            result_dict.update(dict((k, tuple(v)) for k, v in temp_dict.iteritems()))
            
        return result_dict
    