import MonitorApi from "../rest_api/MonitorApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {
    const debugModal = document.getElementById('debug-modal');
    const saveButton = document.getElementById('button-save');
    const isPacked = await window.appState.isPacked();
    let isUserStartingServer;

    debugModal.addEventListener('hidden.bs.modal', () => window.location.reload());
    saveButton.addEventListener('click', async () => {
        let port = document.getElementById('input-port').value;
        port = Number(port);

        if(isNaN(port) || port <= 0 || port > 65535)
            alert('Port must be number between 1 and 65535');
        else {
            await window.appState.set("serverPort", port);
            window.router.routeToFileExplorer();
        }
    });

    if(!isPacked) {
        isUserStartingServer = confirm(`Debug environment detected. 
                                        You can configure the server port manually. 
                                        But configuring the port manually won't spawn a new server process.
                                        If you do that, you have to run the server proccess yourself.
                                        If you don't accept this message, client will try to spawn server process with random port.`);
    }

    if(isUserStartingServer) {
        const modal = new bootstrap.Modal(debugModal);
        modal.show();
    }
    else {
        while(true) {
            const port = await window.appState.get("serverPort");
            const pid = await window.appState.get("serverPid");

            if (!port || !pid) // Process is running for the first time.
                await window.serverProcess.run(); // Wait untill the initialize but not the exit.
            else {
                const monitorApi = new MonitorApi("localhost", port);
                console.log(`requesting pid ${pid} from port ${port}`);
                const heartbeat = await monitorApi.heartbeat();

                if (!heartbeat) // Server was ran previously but not running now.
                    await window.serverProcess.run(); // Wait untill the initialize but not the exit.
                else if (heartbeat.pid === pid)
                    window.router.routeToFileExplorer();
            }

            // Wait for 2 seconds before next check.
            await new Promise(resolve => setTimeout(resolve, 2000));
        }
    }
});