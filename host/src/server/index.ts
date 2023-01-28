import express from "express";
import morgan from "morgan";
import path from "path";

import { getFirmware } from "./firmware_upgrade";
import { initWebSockets } from "./websockets";

const PORT = process.env.PORT || 3000;
const STATIC_DIR = path.join(__dirname, "../../static");

export default function httpServer() {
  const app = express();

  // Logging
  app.use(morgan("tiny"));

  // Static file serving
  app.use("/static", express.static(path.join(__dirname, STATIC_DIR)));

  // Routes
  app.get("/firmware", getFirmware);

  // WebSocket
  initWebSockets(app);

  // Start server
  app.listen(PORT, () => {
    console.log(`🚀 Server at localhost:${PORT}`);
  });
}
