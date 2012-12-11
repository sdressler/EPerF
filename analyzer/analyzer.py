from flask import Flask, render_template, g, request, jsonify

from Analyzer import db_query, filter

app = Flask(__name__)

@app.route('/')
def hello_world():
    db = db_query.db_query()
    
    kernels = db.db_query_kernels()
    devices = db.db_query_devices()
    
    return render_template('index.html', kernels=kernels, devices=devices)

@app.route('/get_data')
def get_data():
    db = db_query.db_query()
    
    devices = request.args.get('devices', [], type=list)
    kernels = request.args.get('kernels', [], type=list)
    
    return jsonify(result=db.db_query_data_table(devices, kernels))


if __name__ == '__main__':
    app.debug = True
    app.run()