import React, { useState, useEffect } from 'react';
// import firebase from 'firebase/app';
import 'firebase/database';
import { getDatabase, ref, onValue , set} from 'firebase/database';
import { infoNotification, successNotification, errorNotification } from './notification';


const Dashboard = () => {
 
 const [doorStatus, setDoorStatus] = useState('false');
  const [ledStatus, setLedStatus] = useState('false');
  const [sensorData, setSensorData] = useState([]);

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
      });
    } catch (error) {
      console.log('Error fetching data:', error);
    }
  };

  const updateLedStatus = (newStatus) => {
    try {
      set(utilsRef('led'), newStatus ? 'true' : 'false')
        .then(() => {
          setLedStatus(newStatus);
          successNotification('LED status updated successfully');
        })
        .catch(() => errorNotification('Failed to update LED status'));
    } catch (error) {
      console.log(error.message);
      errorNotification('Failed to update LED status');
    }
  };

  const updateDoorStatus = (newStatus) => {
    try {
      set(utilsRef.ref('door'), newStatus ? true :false)
        .then(() => {
          setDoorStatus(newStatus ? 'true' : 'false');
          successNotification('Door status updated successfully');
        })
        .catch((error) =>{
          console.log(error);
          errorNotification('Failed to update door status')});
    } catch (error) {
          console.log(error);
          errorNotification('Failed to update door status');
    }
  };
  useEffect(() => {
    // Fetch initial LED and door status from the database and update the state variables
    const fetchInitialStatus = async () => {
      try {
        const ledSnapshot = await utilsRef.child('led').get();
        const doorSnapshot = await utilsRef.child('door').get();

        const ledStatus = ledSnapshot.val() === 'true';
        const doorStatus = doorSnapshot.val() === 'true';

        setLedStatus(ledStatus);
        setDoorStatus(doorStatus);
      } catch (error) {
        console.error('Failed to fetch initial LED and door status:', error);
      }
    };

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
        <h1>Get Data from Firebase Realtime Database</h1>
        {/* <p style={{ color: doorStatus ? 'green' : 'red' }}>
          The door status is: {doorStatus ? 'Door is open' : 'Door is closed'}
        </p>
        <p style={{ color: ledStatus ? 'green' : 'red' }}>
          The LED status is: {ledStatus ? 'LED is on' : 'LED is off'}
        </p> */}
        <button onClick={() => updateLedStatus(true)}>Turn LED On</button>
        <button onClick={() => updateLedStatus(false)}>Turn LED Off</button>
        <button onClick={() => updateDoorStatus(true)}>Open Door</button>
        <button onClick={() => updateDoorStatus(false)}>Close Door</button>
      </div>


      <div>
        <h1>Realtime Database Sensor Data</h1>
        {sensorData.map((record) => (
          <div key={record.id}>
            <p>Timestamp: {record.id}</p>
            <p>Flame: {record?.sensor?.flame ? "true" : "false"}</p>
            <p>Gas: {record?.sensor?.gas}</p>
            <p>LDR: {record?.sensor?.ldr}</p>
            <p>PIR: {record?.sensor?.pir ? "true" : "false"}</p>
            <p>Ultrasonic: {record.ultrasonic ? "true" : "false"}</p>
          </div>
        ))}
      </div>
    </div>
  );
};

export default Dashboard;