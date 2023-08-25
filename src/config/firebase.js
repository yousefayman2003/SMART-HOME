import firebase from 'firebase/app';
import 'firebase/database';
import { initializeApp } from 'firebase/app';
import { getDatabase, ref, onValue } from 'firebase/database';
import { getAuth } from "firebase/auth";

// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyAzkKax2O0i-2b2r8U13HjayCipQmChB3E",
  authDomain: "esp-firebase-641b5.firebaseapp.com",
  databaseURL: "https://esp-firebase-641b5-default-rtdb.firebaseio.com",
  projectId: "esp-firebase-641b5",
  storageBucket: "esp-firebase-641b5.appspot.com",
  messagingSenderId: "225258316252",
  appId: "1:225258316252:web:4b2e40758fad8f0fb86f19",
  measurementId: "G-8863DRT5J5"
};


const app = initializeApp(firebaseConfig);
const database = getDatabase(app);
export const auth = getAuth(app);
