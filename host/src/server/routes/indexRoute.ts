import { Request, Response } from "express";
import { getFirmwareVersion } from "./firmware";

export const indexRoute = async (req: Request, res: Response) => {
  const { version } = require(`${__dirname}../../../../package.json`);
  const firmwareVersion = await getFirmwareVersion();
  res.send(`Disco Server v${version}, firmware: ${firmwareVersion || "?.?.?"}`);
};
