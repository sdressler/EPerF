from flask import Flask, render_template, g, request, jsonify

from Analyzer import db_query

app = Flask(__name__)

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.route('/get_experiments')
def get_experiments():
    db = db_query.db_query();
    return jsonify(result=db.db_query_experiments())

@app.route('/get_experiment_overview')
def get_experiment_overview():
    db = db_query.db_query()
    e_id = request.args.get('id', "", type=str)
    return jsonify(result=db.db_query_experiment_overview(e_id))

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
    start_time = request.args.get('start_time', 0.0, type=float)
    stop_time = request.args.get('stop_time', 0.0, type=float)
    
    return jsonify(result=db.db_query_data_table(
        devices, kernels,
        start_time, stop_time
    ))


if __name__ == '__main__':
    app.debug = True
    #app.run(host='0.0.0.0', port=80)
    app.run()