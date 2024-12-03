import subprocess
from flask import Flask, render_template, request, jsonify
#from database_final import create_table, insert_data, get_sensor_data
import json

JSON_FILE = "data_pipe.json"
subprocess.Popen(["sudo", "./.venv/bin/python", "./ble.py"])

app = Flask(__name__)

#create_table()

def read_json():
    try:
        with open(JSON_FILE, "r") as file:
            return json.load(file)
    except FileNotFoundError:
        return []  # Retourner une liste vide si le fichier n'existe pas
    except json.JSONDecodeError:
        return []  # Retourner une liste vide si le contenu est invalide
        
#def write_json(data):
#    try:
#        with open(JSON_FILE, "r") as file:
#            json.dump(data, file, indent=4)
#    except Exception as e:
#        print(f"Erreur lors de l'écriture du fichier JSON : {e}")

@app.route('/')
def home():
    sensor_data = read_json()
    return render_template('index.html', sensor_data=sensor_data)

#@app.route('/update-sensor', methods=['POST'])
#def update_sensor():
#    try:
#        # Récupérer les données JSON envoyées par la Raspberry Pi
#        data = request.json
#        if not data:
#            return jsonify({'status': 'error', 'message': 'Aucune donnée reçue'}), 400
#        
#        # Sauvegarder les données dans la base de données
#        sensor_data = read_json()
#        sensor_data.append(data)
#        write_json(sensor_data)
#        
#        return jsonify({'status': 'success'})
#    except Exception as e:
#        print(f"Erreur lors du traitement : {e}")
#        return jsonify({'status': 'error', 'message': 'Erreur serveur'}), 500

@app.route('/get-sensor-data', methods=['GET'])
def get_sensor_data_route():
     try:
        # Récupérer les données de la base de données
        data = read_json()
        # Retourner les données sous format JSON
        return jsonify(data)
     except Exception as e:
         print(f"Erreur lors de la récupération des données : {e}")
         return jsonify({'status': 'error', 'message': 'Erreur serveur'}), 500

#if __name__ == '__main__':
#    app.run(debug=False, host='0.0.0.0', port=5000)
