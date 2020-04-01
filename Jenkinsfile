pipeline {
	agent { label "devel-image && x86_64" }

	parameters {
		booleanParam name: 'RUN_TESTS', defaultValue: true, description: 'Run Tests?'
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

        stage('Install') {
            steps {
                cmakeBuild buildType: 'Release',
                cleanBuild: true,
                installation: 'InSearchPath',
                steps: [[args: 'install']]
            }
        }

        stage('Tests') {
            when {
                environment name: 'RUN_TESTS', value: 'true'
            }
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