FROM node:18-alpine

# Install ngrok
USER root
RUN wget https://bin.equinox.io/c/bNyj1mQVY4c/ngrok-v3-stable-linux-arm64.tgz
RUN tar xvzf ngrok-v3-stable-linux-arm64.tgz -C /usr/local/bin

# Run node
USER node
WORKDIR /app

RUN ngrok config add-authtoken 28iWb7VdAva2WRNquDKPfPeAz0c_4tcN42AhrrERLxvi1YM69

COPY --chown=node . .
RUN yarn install

CMD ["yarn", "dev"]
