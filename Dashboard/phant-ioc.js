(function() {

  const TYPE_INFO = {
    type: 'ioc-phant-datasource',
    name: 'IOC Phant',
    kind: 'datasource',
    author: 'Steve Knable',
    version: '1.0.0',
    description: 'Grab data from Phant stream for IOC',
    dependencies: [
            "https://code.jquery.com/jquery-1.12.4.min.js"
            ],
    settings: [
        {
          id: "url_public-ioc",
          name: "url_public-ioc",
          description: "Phant Public html Key for IOC",
          type: "string"
        }
    ]
  };

  class Plugin {

    constrctor(props) {
      // Any code that is required to execute when the plugin is loaded the first time e.g. setting up some properties
      // Set how often fetchData() will be called, 'Infinite' to disable regular updates
      props.setFetchInterval(10 * 10000); // Fetch every 10 seconds, and once after the plugin is loaded.
      props.setFetchReplaceData(true);
    }

    fetchData(fulfill, reject) {

      const settingValues = this.props.state.settings;
      $.getJSON(settingValues["url_public"], function(json) {

            fulfill(json);
      });


    }
  }

  // Now we just have to handover our TYPE_INFO object and Plugin class to the dashboard API to load them
  window.iotDashboardApi.registerDatasourcePlugin(TYPE_INFO, Plugin)
})();
