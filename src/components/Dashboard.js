import React, { useState, useEffect } from 'react';
// import firebase from 'firebase/app';
import 'firebase/database';
import { getDatabase, ref, onValue , set, get} from 'firebase/database';
import { infoNotification, successNotification, errorNotification } from './notification';


const Dashboard = () => {
 
 const [doorStatus, setDoorStatus] = useState('false');
  const [ledStatus, setLedStatus] = useState('false');
  const [sensorData, setSensorData] = useState([]);
  const [emergency, setEmergency] = useState({
    invalid_password: false,
    theif: false,
  });
  const [lastSensorReading, setLastSensorReading] = useState({});
  const database = getDatabase();
  const utilsRef = ref(database, 'utils');
  const sensorRef =ref(database, 'reading')

  console.log(utilsRef); // Check if it's null
  console.log(sensorRef); // Check if it's null

  const fetchData = async () => {
    try {
      const db = getDatabase();
      const utilsRef = ref(db, 'utils');
      const sensorRef = ref(db, 'reading');

      onValue(utilsRef, (snapshot) => {
        const data = snapshot.val();
        setDoorStatus(data.door);
        setLedStatus(data.led);
      });
  

      onValue(sensorRef, (snapshot) => {
        const data = snapshot.val();
        console.log('Received data:', data);
        const dataArray = Object.entries(data).map(([key, value]) => ({ id: key, ...value }));
        console.log('Transformed data:', dataArray); // Check the transformed data
        setSensorData(dataArray);
        
        if (dataArray.length > 0) {
          const lastReading = dataArray[dataArray.length - 1];
          setLastSensorReading(lastReading);
        }
         // Check if the gas reading is more than 1100
         if (lastSensorReading.sensor?.gas > 1100) {
          errorNotification('Gas reading is above the threshold (1100)');
        }
        if (dataArray.length > 0 && dataArray[dataArray.length - 1].ultrasonic) {
          errorNotification('Ultrasonic sensor value is true');
        }
      });
    } catch (error) {
      console.log('Error fetching data:', error);
    }
  };

  // if (emergency.invalid_password || emergency.theif) {
  //   errorNotification('Emergency condition detected: Invalid password and there is a thief in the house.');
  // }
  
  const updateLedStatus = (newStatus) => {
    try {
      set(ref(database, 'utils/led'), newStatus ? true : false)
        .then(() => {
          setLedStatus(newStatus);
          successNotification('LED status updated successfully');
        })
        .catch(() => errorNotification('Failed to update LED status'));
  
      // checkEmergencyConditions(newStatus, doorStatus);
    } catch (error) {
      console.log(error.message);
      errorNotification('Failed to update LED status');
    }
  };
  
  const updateDoorStatus = (doorStatus) => {
    try {
      set(ref(database, 'utils/door'), doorStatus ? true : false)
        .then(() => {
          setDoorStatus(doorStatus ? 'true' : 'false');
          successNotification('Door status updated successfully');
        })
        .catch((error) => {
          console.log(error);
          errorNotification('Failed to update door status');
        });
  
      // checkEmergencyConditions(ledStatus, doorStatus);
    } catch (error) {
      console.log(error);
      errorNotification('Failed to update door status');
    }
  };
  useEffect(() => {
    // Check if either invalid_password or theif is true
    if (emergency.invalid_password || emergency.theif) {
      errorNotification('Emergency condition detected: Invalid password or there is a thief in the house.');
    }
  }, [emergency]);

  useEffect(() => {
    // Fetch initial LED and door status from the database and update the state variables
    const fetchInitialStatus = () => {
      try {
        onValue(ref(utilsRef, 'led'), (snapshot) => {
          const ledStatus = snapshot.val() === 'true';
          setLedStatus(ledStatus);
        });
  
        onValue(ref(utilsRef, 'door'), (snapshot) => {
          const doorStatus = snapshot.val() === 'true';
          setDoorStatus(doorStatus);
        });
      } catch (error) {
        console.error('Failed to fetch initial LED and door status:', error);
      }
    };
  


    //Update the state of the emergency
  setEmergency((prevState) => ({
    ...prevState,
    invalid_password: true,
    theif: true,


  }));






    fetchInitialStatus();

      
  }, []);
  useEffect(() => {
    console.log('useEffect triggered');
    fetchData();
    // infoNotification("Welcome !!")
  }, []); 
  


 // Render the sensorData state in my document
  return (
    <div>
     <div
        style={{
          width: '70%',
          margin: '0 auto',
          boxShadow: '0 0 5px rgba(0, 0, 0, 0.3)',
          padding: '20px',
          backgroundColor: '#f0f0f0',
          fontFamily: 'Arial, sans-serif',
        }}
      >
        <h1>control on Led and Door</h1>
       
        <div style={{
          width: '70%',
          margin: '0 auto',
          boxShadow: '0 0 5px rgba(0, 0, 0, 0.3)',
          padding: '20px',
          backgroundColor: '#f0f0',
          fontFamily: 'Arial, sans-serif',
          display: 'flex',
          justifyContent: 'center',
        }}>
        <button onClick={() => updateLedStatus(true)}>Turn LED On</button>
        <button onClick={() => updateLedStatus(false)}>Turn LED Off</button>
        <button onClick={() => updateDoorStatus(true)}>Open Door</button>
        <button onClick={() => updateDoorStatus(false)}>Close Door</button>
        </div>
      </div>


      <div>
  <h1>Last Sensor Reading</h1>
  {lastSensorReading && lastSensorReading.id ? (
    <div>
      <p>Timestamp: {lastSensorReading.id}</p>
      {/* <p>Flame: {lastSensorReading?.sensor?.flame ? "true" : "false"}</p> */}
      <p>Gas: {lastSensorReading?.sensor?.gas}</p>
      {/* <p>LDR: {lastSensorReading?.sensor?.ldr}</p> */}
      <p>PIR: {lastSensorReading?.sensor?.pir ? "true" : "false"}</p>
      <p>Ultrasonic: {lastSensorReading.ultrasonic ? "true" : "false"}</p>
    </div>
  ) : (
    <p>No sensor data available</p>
  )}
</div>
    </div>
  );
};

export default Dashboard;