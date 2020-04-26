import json
import math
import s3_io as s3
print('Loading function')

esp_1 = {
    "distance": 0,
    "X": 0,
    "Y": 0
}

esp_2 = {
    "distance": 0,
    "X": 10,
    "Y": 0
}

esp_3 = {
    "distance": 0,
    "X": 0,
    "Y": 10
}

TXPower = -6


def lambda_handler(event, context):
    print("Received event: " + json.dumps(event['state']['reported']))
    
    rssi = event['state']['reported']['RSSI']
    power = (int(TXPower)-int(rssi))/20
    distance = pow(10,power)
    distance = math.floor(distance/1000)
    print("Calculated distance : " + str(distance))
    
    X = 0
    Y = 0
    
    if event['state']['reported']['Server'] == "Server1":
        esp_1["distance"] = distance
    elif event['state']['reported']['Server'] == "Server2":
        esp_2["distance"] = distance
    else: 
        esp_3["distance"] = distance
        
    if esp_1["distance"] != 0 and esp_2["distance"] != 0 and esp_3["distance"] != 0:
        X = (pow(esp_1["distance"], 2) - pow(esp_2["distance"], 2) + 100)/20
        Y = (pow(esp_1["distance"], 2) - pow(esp_3["distance"], 2) + 100)/20
        print("Distance 1 :  " + str(esp_1["distance"]))
        print("Distance 2 :  " + str(esp_2["distance"]))
        print("Distance 3 :  " + str(esp_3["distance"]))
        print(str(X) + "," + str(Y))
        data = s3.read_from_s3()
        data = data + "\n" + str(X) + "," + str(Y) 
        s3.write_to_s3(data)
    else:
        print("Not found")
        print(esp_1)
        print(esp_2)
        print(esp_3)
