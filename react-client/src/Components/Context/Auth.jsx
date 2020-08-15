import { createContext, useContext } from 'react';


export function useAuth() {
  return useContext(AuthContext);
}

export const AuthContext = createContext({userName: "", password: ""});
