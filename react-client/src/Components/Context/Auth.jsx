import { createContext, useContext } from 'react';
//uint32_t id;
//std::string username;
//std::string password;
//uint32_t privilege;

export function useAuth() {
  return useContext(AuthContext);
}

export const AuthContext = createContext({userName: "", password: ""});
