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
        
    def db_query_kernels(self):
        return self.db_query_full_table("kernels");
    
    def db_query_devices(self):
        return self.db_query_full_table("devices");
    
    def db_query_data_table(self, device_ids, kernel_ids, start_time, stop_time):
        devices = ""
        kernels = ""
        
        for did in device_ids:
            devices += " id_device = " + did + " OR"
            
        for kid in kernel_ids:
            kernels += " id_kernel = " + kid + " OR"
            
        query = "SELECT \
                    tid, \
                    ts_start_s - ts_start_ns * 1.0e-9, \
                    ts_stop_s - ts_stop_ns * 1.0e-9 \
                 FROM data WHERE" + devices + kernels
                 
        # Remove the last " OR"
        query = query[:len(query) - 3]
        
        query += " ORDER BY tid"
        
        '''
        if (start_time > 0.0):
            query += " AND ts_start_s - ts_start_ns * 1.0e-9 > " + str(start_time)
                    
        if (stop_time > 0.0):
            query += " AND ts_stop_s - ts_stop_ns * 1.0e-9 < " + str(stop_time)
        '''
        
        return self.db_query(query)
    