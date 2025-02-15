import serial
import time
from tkinter import Tk, Label, Button
from biosppy.signals import ecg
from sklearn.metrics.pairwise import cosine_similarity
from scipy.spatial.distance import euclidean
import numpy as np
import os
import json

# Serial port setup
arduino_port = '/dev/ttyACM0'
baud_rate = 115200
arduino = serial.Serial(arduino_port, baud_rate, timeout=1)

# Directory for saving user templates
TEMPLATES_DIR = "ecg_templates"
os.makedirs(TEMPLATES_DIR, exist_ok=True)

# Global variables
user_id_counter = 0  # Starting user ID

# Authentication
def authenticate():
    arduino.write(b'AUTH\n')  # Send the AUTH command to Arduino

    print("Waiting for Arduino responses...")  # Debug message
    while True:
        response = arduino.readline().decode('utf-8').strip()
        if not response:
            continue  # Ignore empty responses
        print(f"Received response from Arduino: {response}")  # Debug message

        # Check if the response contains USER_ID
        if response.startswith("USER_ID:"):
            user_id = response.split(":")[1].strip()
            print(f"Fingerprint authentication successful. User ID: {user_id}")

            # Load the corresponding template
            user_template = load_template(user_id)
            if user_template is None:
                print(f"Error: No template found for User ID {user_id}.")
                arduino.write(b"MATCH_FAIL\n")  # Notify Arduino of failure
                return
        
            # Receive new ECG data and compare it to the saved template
            auth_template = receive_ecg_data()
            results, match = compare_templates_cosine(user_template, auth_template)
            
            if match:
                print("Authentication successful!")
                arduino.write(b"MATCH_SUCCESS\n")  # Notify Arduino of success
            else:
                print("Authentication failed.")
                arduino.write(b"MATCH_FAIL\n")  # Notify Arduino of failure
            break  # Exit the loop after processing

        # Check for known error messages
        elif response.startswith("ERROR:"):
            print(f"Fingerprint authentication failed. Response: {response}")
            arduino.write(b"MATCH_FAIL\n")  # Notify Arduino of failure
            break  # Exit the loop on error

        # Handle unexpected responses or intermediate debug messages
        else:
            pass


# Function to load the template
def load_template(user_id):
    file_path = os.path.join(TEMPLATES_DIR, f"user_{user_id}.json")
    try:
        with open(file_path, "r") as file:
            template = json.load(file)
        return template
    except FileNotFoundError:
        print(f"Template not found for User ID {user_id}. File path: {file_path}")
        return None
    except Exception as e:
        print(f"Error loading template for User ID {user_id}: {e}")
        return None
        

def compare_templates_cosine(template1, template2, thresholds=None):
    """
    Compare two ECG templates using Cosine Similarity across multiple features.
    """
    if thresholds is None:
        thresholds = {"rpeaks": 0.8, "heart_rate": 0.8, "templates": 0.8}

    results = {}
    match = True

    # Helper function to compute cosine similarity
    def compute_similarity(vector1, vector2):
        try:
            vector1 = np.array(vector1).ravel().reshape(1, -1)
            vector2 = np.array(vector2).ravel().reshape(1, -1)
            similarity = cosine_similarity(vector1, vector2)[0, 0]
            return similarity
        except Exception as e:
            print(f"Error computing cosine similarity: {e}")
            return None

    # Compare R-peaks
    rpeaks1 = template1.get("rpeaks", [])
    rpeaks2 = template2.get("rpeaks", [])
    if rpeaks1 and rpeaks2:
        similarity_rpeaks = compute_similarity(rpeaks1, rpeaks2)
        if similarity_rpeaks is not None:
            results["rpeaks"] = similarity_rpeaks
            if similarity_rpeaks < thresholds["rpeaks"]:
                match = False
        else:
            results["rpeaks"] = None
            match = False
    else:
        print("Missing R-peaks data. Skipping R-peaks comparison.")
        results["rpeaks"] = None
        match = False

    # Compare Heart Rate
    heart_rate1 = template1.get("heart_rate", [])
    heart_rate2 = template2.get("heart_rate", [])
    
    len1 = len(heart_rate1)
    len2 = len(heart_rate2)
    
    # Determine the minimum length
    min_length = min(len1, len2)

    # Trim both lists to the same length
    heart_rate1 = heart_rate1[:min_length]
    heart_rate2 = heart_rate2[:min_length]
    
    if heart_rate1 and heart_rate2:
        similarity_hr = compute_similarity(heart_rate1, heart_rate2)
        if similarity_hr is not None:
            results["heart_rate"] = similarity_hr
            if similarity_hr < thresholds["heart_rate"]:
                match = False
        else:
            results["heart_rate"] = None
            match = False
    else:
        print("Missing Heart Rate data. Skipping Heart Rate comparison.")
        results["heart_rate"] = None
        match = False

    # Compare Templates
    templates1 = template1.get("templates", [])
    templates2 = template2.get("templates", [])
    if templates1 and templates2:
        try:
            # Flatten templates
            templates1_flat = np.concatenate(templates1).ravel()
            templates2_flat = np.concatenate(templates2).ravel()

            similarity_templates = compute_similarity(templates1_flat, templates2_flat)
            if similarity_templates is not None:
                results["templates"] = similarity_templates
                if similarity_templates < thresholds["templates"]:
                    match = False
            else:
                results["templates"] = None
                match = False
        except Exception as e:
            print(f"Error comparing Templates: {e}")
            results["templates"] = None
            match = False
    else:
        print("Missing Templates data. Skipping Templates comparison.")
        results["templates"] = None
        match = False

    return results, match

# Enrollment
def enroll():
    global user_id_counter
    command = f"ENROLL {user_id_counter}\n"
    arduino.write(command.encode())

    while True:
        response = arduino.readline().decode('utf-8').strip()
        if response:
            print(f"Arduino: {response}")
            if response == "ENROLL_SUCCESS":
                
                # Receive and save the ECG template
                template = receive_ecg_data()
                if template is None:
                    print("Enrollment failed. Insufficient ECG data received.")
                    return
                
                file_path = os.path.join(TEMPLATES_DIR, f"user_{user_id_counter}.json")
                print(f"Template: {template}")
                save_template(template, file_path)

                print(f"Template saved for User ID {user_id_counter} at {file_path}")
                user_id_counter += 1
                break
            elif "ERROR" in response:
                print(f"Error during enrollment. {response}")
                break



# Function to receive ECG data from Arduino and reconstruct the blocks
def receive_ecg_data():
    """
    Receive ECG data from Arduino, reconstruct it into blocks, and return the processed ECG signal.
    """
    arduino.write(b'ECG\n')
    ecg_data = []  # List to store the complete ECG signal
    print("Receiving ECG data...")
    
    while True:
        data = arduino.readline().decode('utf-8').strip()
        
        # Ignore non-numerical lines
        if data.startswith("FREQ:"):
            # Frequency message received, stop receiving blocks
            frequency = int(data.split(":")[1])
            break
        elif not data or not data[0].isdigit():
            continue
        else:
            try:
                # Parse the ECG data block and extend the main list
                block = list(map(int, data.split(',')))
                ecg_data.extend(block)
                print(f"Total: {len(ecg_data)} samples.")
            except ValueError as e:
                print(f"Skipping invalid data block: '{data}' - Error: {e}")

    if len(ecg_data) < 4503:  # Minimum length required for filtering
        print("Error: Insufficient ECG data received for processing.")
        return None

    # **Commented out the voltage conversion (as previously requested)**
    # ecg_data_volt = np.array(ecg_data) * (3.3 / 1023)

    # Process the signal using biosppy (with sufficient data)
    out = ecg.ecg(signal=np.array(ecg_data), sampling_rate=frequency, show=False)
    
    # Return a processed ECG template
    return {
        "templates": [out["filtered"]],  # Filtered ECG signal
        "rpeaks": out["rpeaks"].tolist(),  # Detected R-peaks
        "heart_rate": out["heart_rate_ts"].tolist(),  # Heart rate timestamps
    }


# Function to save the template to a file
def save_template(template, file_path):
    try:
        # Convert all NumPy arrays to lists and ensure all data is JSON serializable
        def make_serializable(obj):
            if isinstance(obj, np.ndarray):  # Convert NumPy array to list
                return obj.tolist()
            elif isinstance(obj, dict):  # Recursively handle nested dictionaries
                return {key: make_serializable(value) for key, value in obj.items()}
            elif isinstance(obj, list):  # Recursively handle lists
                return [make_serializable(value) for value in obj]
            else:
                return obj  # Return the object if already serializable

        serializable_template = make_serializable(template)

        with open(file_path, "w") as file:
            json.dump(serializable_template, file, indent=4)
    except Exception as e:
        print("Serialized Template:", serializable_template)
        print(f"Failed to save template: {e}")

# Exit
def exit_system():
    global arduino  # Reuse the global `arduino` variable
    print("Exiting...")
    
    # Safely close the existing serial port
    if arduino.is_open:
        arduino.close()

    # Clear any global variables or states
    global user_id_counter
    user_id_counter = 0  # Reset user ID counter

    # Restart Arduino and reinitialize serial connection
    print("Reinitializing Arduino...")
    time.sleep(2)  # Allow time for Arduino to reset

    # Reopen the serial port
    arduino.open()

    print("Arduino restarted and system reset.")

# Main loop handling EXIT
def main_loop():
    print("Waiting for Arduino to initialize...")
    
    while True:
        response = arduino.readline().decode('utf-8').strip()
        if response == "Arduino ready for commands.":
            print("Arduino is ready for commands.")
            break

    print("Listening for commands from Arduino...")

    while True:
        try:
            command = arduino.readline().decode('utf-8').strip()
            if not command:
                continue

            print(f"Received command from Arduino: {command}")

            if command == "AUTH":
                authenticate()
            elif command == "ENROLL":
                enroll()
            elif command == "EXIT":
                print("Exiting system as per Arduino command.")
                exit_system()  # Call the updated exit_system function
            else:
                pass

        except KeyboardInterrupt:
            print("Keyboard interrupt received. Exiting...")
            exit_system()
            break
        except Exception as e:
            print(f"Error in main loop: {e}")


# Start the main loop
if __name__ == "__main__":
    main_loop()


