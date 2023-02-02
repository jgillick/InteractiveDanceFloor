import { Router } from "express";

import { getFirmwareRoute } from "./firmware";
import { indexRoute } from "./indexRoute";

export const routes = Router();

routes.get("/", indexRoute);
routes.get("/firmware", getFirmwareRoute);
