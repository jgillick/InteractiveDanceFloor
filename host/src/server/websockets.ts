import { Express } from "express";
import expressWs from "express-ws";
import { WebSocket } from "ws";

let state = 0;

export function initWebSockets(app: Express) {
  const webservice = expressWs(app);

  webservice.app.ws("/", (ws) => {
    // New client
    ws.send(String(state), { binary: false });

    ws.on("message", (data) => {
      const msg = data.toString();
      console.log(`Received: ${msg}`);

      if (msg == "toggle") {
        const toggled = state ? 0 : 1;
        console.log({ previous: state, new: toggled });

        state = toggled;

        const clients = webservice.getWss().clients;
        clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN) {
            client.send(String(state), { binary: false });
          }
        });
      }
    });
  });
  console.log("🔌 WebSockets started");
}
