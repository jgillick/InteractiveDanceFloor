import { Request, Response } from "express";
import { promises as fs } from "fs";
import path from "path";
import semver from "semver";

const FIRMWARE_PATH = `/static/firmware.bin`;
const VERSION_OFFSET = 48;
const VERSION_LEN = 5;

/**
 * Read the firmware version from the file
 */
export async function getFirmwareVersion(): Promise<string | null> {
  const filepath = path.join(__dirname, "../../..", FIRMWARE_PATH);
  const buffer = Buffer.alloc(VERSION_LEN);
  let version: string | null = null;

  // Check if the file exists and can be read
  try {
    await fs.access(filepath, fs.constants.R_OK);
  } catch (error) {
    console.warn("No firmware file present");
    return null;
  }

  let fd;
  try {
    fd = await fs.open(filepath, "r");
    await fd.read(buffer, 0, VERSION_LEN, VERSION_OFFSET);
    version = buffer.toString("utf8");
  } catch (err) {
    console.error(err);
  }
  if (fd) {
    fd.close();
  }
  return version;
}

/**
 * GET route that compares the version query string to the latest firmware file.
 * If the version query is less than the latest firmware, it redirects to the firmware file.
 * Otherwise it returns status 200.
 */
export const getFirmwareRoute = async (req: Request, res: Response) => {
  const versionParam = req.query.version;
  const appVersion = await getFirmwareVersion();

  const response = {
    appVersion,
    updateAvailable: 0,
    firmwarePath: FIRMWARE_PATH,
  };

  if (!versionParam) {
    res.status(400).send("ERROR: Version query param required");
  } else if (!semver.valid(versionParam)) {
    res.status(400).send("ERROR: Invalid version string");
  } else {
    const updateAvailable =
      appVersion &&
      semver.valid(appVersion) &&
      semver.gt(appVersion, versionParam);
    response.updateAvailable = updateAvailable ? 1 : 0;

    res.status(200).send(response);
  }
};
