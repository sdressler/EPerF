'''
Created on Dec 11, 2012

@author: bzcdress
'''

class filter:
    def __init__(self):
        None
        
    def filter_data(self, data):
        
        #for d in data:
        #    print d
            
        #data_filt = dict({ 'start_time': float(data['ts_start_s']) + float(data['ts_start_ns']) * 1.0e-9 })
        data_filt = []
        for d in data:
            
            start_time = float(d['ts_start_s']) + float(d['ts_start_ns'] * 1.0e-9)
            runtime = float(d['ts_stop_s']) + float(d['ts_stop_ns']) * 1.0e-9 - start_time
            
            data_filt.append({
                'start_time': start_time,
                'runtime': runtime,
                'thread': d['tid']
            });
            
        
        return data_filt