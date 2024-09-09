# -*- coding: utf-8 -*-
"""
Created on Wed Jul  3 16:22:25 2024

@author: vscgo
"""
from flask import Flask, jsonify, render_template
import gspread
from oauth2client.service_account import ServiceAccountCredentials

app = Flask(__name__)

# Define the scope and credentials
scope = ['https://spreadsheets.google.com/feeds',
         'https://www.googleapis.com/auth/drive']
creds = ServiceAccountCredentials.from_json_keyfile_name(r"C:\Users\vscgo\OneDrive\Desktop\scriptapp\co2-sensor-428405-2e5755728d86.json", scope)
client = gspread.authorize(creds)

# Define the Google Sheet
sheet = client.open("co2 sensor").sheet1  # Replace with your actual sheet name

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/sensor-data', methods=['GET'])
def get_sensor_data():
    # Define the expected headers based on your sheet
    expected_headers = ["date", "time", "node1", "node2", "node3", "node4"]  # Update these as per your actual headers
    
    # Fetch all records using expected headers
    data = sheet.get_all_records(expected_headers=expected_headers)
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)
