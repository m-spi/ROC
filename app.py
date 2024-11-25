from flask import Flask, render_template, request, jsonify
#from database_final import create_table, insert_data, get_sensor_data

app = Flask(__name__)

# Initialiser la base de données et créer la table si nécessaire
#create_table()

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/update-sensor', methods=['POST'])
def update_sensor():
    try:
        # Récupérer les données JSON envoyées par la Raspberry Pi
        data = request.json
        if not data:
            return jsonify({'status': 'error', 'message': 'Aucune donnée reçue'}), 400
        
        # Sauvegarder les données dans la base de données
       # insert_data(data)
        
        return jsonify({'status': 'success'})
    except Exception as e:
        print(f"Erreur lors du traitement : {e}")
        return jsonify({'status': 'error', 'message': 'Erreur serveur'}), 500

# @app.route('/get-sensor-data', methods=['GET'])
# def get_sensor_data_route():
    # #try:
    #     # Récupérer les données de la base de données
    #     #data = get_sensor_data()
    #     # Retourner les données sous format JSON
    #     return jsonify(data)
    # except Exception as e:
    #     print(f"Erreur lors de la récupération des données : {e}")
    #     return jsonify({'status': 'error', 'message': 'Erreur serveur'}), 500

if __name__ == '__main__':
    app.run(debug=True)






