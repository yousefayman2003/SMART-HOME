import{auth} from '../config/firebase';
import { createUserWithEmailAndPassword } from "firebase/auth";
import { useState } from 'react';
import "./auth.css";
import { useNavigate } from 'react-router-dom';

export const Auth = () =>{
    const[email,setEmail]=useState("");
    const[password,setPassword]=useState(""); 
    const [showAlert, setShowAlert] = useState(false);
    const navigate = useNavigate();
const signIn= async() =>{
try{
    // Sign in the user with the entered email and password
    await createUserWithEmailAndPassword(auth,email,password);
    navigate('/dashboard'); // Navigate to the dashboard page
}catch(err)
{console.log(err);
    setShowAlert(true); // Set showAlert to true on error

}
};

    return(
        <div className='page'>
    <div className='cover'>
    
<input placeholder="Email.." 
onChange={(e) => setEmail(e.target.value)} />


<input placholder="pass.." type='password' 
onChange={(e) => setPassword(e.target.value)}
/>

<button onClick={signIn}>Sign up</button>

{showAlert && (
          <div className='alert'> Please try again.</div>
        )}
        </div>
        </div>
        );


};