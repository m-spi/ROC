import sqlite3
from datetime import datetime

# Fonction pour créer la table 
def create_table():
    try:
        conn = sqlite3.connect('my.db')
        cur = conn.cursor()
        cur.execute('''
            CREATE TABLE IF NOT EXISTS capteurs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                temperature REAL,
                humidite REAL,
                horodatage TEXT
            )
        ''')
        conn.commit()
        conn.close()
        print("Table créée avec succès")
    except sqlite3.Error as error:
        print("Erreur lors de la création de la table :", error)

# Fonction pour insérer des données dans la table
def insert_data(data):
    try:
        conn = sqlite3.connect('my.db')
        cur = conn.cursor()
        # Assurez-vous que les données de la Raspberry Pi contiennent les bonnes clés
        temperature = data["temperature"]
        humidite = data["humidite"]
        horodatage = data["horodatage"]
        
        sql = "INSERT INTO capteurs (temperature, humidite, horodatage) VALUES (?, ?, ?)"
        cur.execute(sql, (temperature, humidite, horodatage))
        conn.commit()
        conn.close()
        print("Données insérées avec succès.")
    except sqlite3.Error as error:
        print("Erreur lors de l'insertion des données :", error)

# Fonction pour récupérer les données des capteurs
def get_sensor_data():
    try:
        conn = sqlite3.connect('my.db')
        cur = conn.cursor()
        cur.execute("SELECT * FROM capteurs ORDER BY horodatage DESC LIMIT 10")  # Limiter à 10 derniers enregistrements
        rows = cur.fetchall()
        conn.close()
        
        # Transformer les résultats en une liste de dictionnaires
        sensor_data = []
        for row in rows:
            sensor_data.append({
                'timestamp': row[3],  # Horodatage
                'temperature': row[1],  # Température
                'humidity': row[2]  # Humidité
            })
        
        return sensor_data
    except sqlite3.Error as error:
        print("Erreur lors de la récupération des données :", error)
        return []

