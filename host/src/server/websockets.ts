import http from "http";
import https from "https";
import { WebSocket, WebSocketServer } from "ws";

let state = 0;

export function initWebSockets(server: https.Server | http.Server) {
  const wss = new WebSocketServer({ server });

  wss.on("connection", (ws) => {
    // New client
    console.log("New client");
    ws.send(String(state), { binary: false });

    ws.on("message", (data) => {
      const msg = data.toString();
      console.log(`Received: ${msg}`);

      if (msg == "toggle") {
        const toggled = state ? 0 : 1;
        console.log({ previous: state, new: toggled });

        state = toggled;

        wss.clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN) {
            client.send(String(state), { binary: false });
          }
        });
      }
    });
  });
}
