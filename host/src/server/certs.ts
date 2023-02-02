import fs from "fs";

const CERT_PEM_PATH = process.env.CERT_PEM_PATH || "cert/cert.pem";
const KEY_PEM_PATH = process.env.KEY_PEM_PATH || "cert/key.pem";

/**
 * Get cert files
 */
export const getCerts = (): { key?: Buffer; cert?: Buffer } => {
  if (!fs.existsSync(CERT_PEM_PATH) || !fs.existsSync(KEY_PEM_PATH)) {
    throw new Error("Certs were not found");
  }

  return {
    cert: fs.readFileSync(CERT_PEM_PATH),
    key: fs.readFileSync(KEY_PEM_PATH),
  };
};
