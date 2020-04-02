pipeline {
	agent { label "devel-image && x86_64" }

	parameters {
        	booleanParam name: 'RUN_MEMCHECKS', defaultValue: true, description: 'Run memchecks?'
	}

	stages {
		stage('Build') {
		    steps {
			cmakeBuild buildType: 'Release',
			cleanBuild: true,
			installation: 'InSearchPath',
			steps: [[withCmake: true]]
		    }
		}

		stage('Tests') {
		    steps {
			cmakeBuild buildType: 'Release',
			cleanBuild: true,
			installation: 'InSearchPath',
			steps: [[args: 'test']]
		    }
		}

		stage('Memchecks') {
		    when {
			environment name: 'RUN_MEMCHECKS', value: 'true'
		    }
		    steps {
			cmakeBuild buildType: 'Release',
			cleanBuild: true,
			installation: 'InSearchPath',
			steps: [[args: 'test memcheck']]
		    }
		}
	}
}
